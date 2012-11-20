#include <iostream>
#include <string>
#include <string.h>
#include <map>
#include <sstream>
#include <atomic>
#include "clientstream.h"
#include "serverstream.h"
#include "network.h"
#include "thread_pool.h"
#include "safe_map.h"
#include "ibstream.h"

enum task_type { CREATE, PAUSE, RESUME, STOP};

struct request
{
  std::atomic<bool> running;

  request():running(true){}
};

class processor
{
public:
  alm::thread_pool pool;
  alm::safe_map<int, alm::clientstream*> clients;
  alm::safe_map<unsigned int, request*> requests;
  alm::inmessage* msg;

  processor():pool(2), msg(0){}

  ~processor()
  {
    clients.for_each([] (alm::clientstream* client)
      {
        client->closeSocket();
        delete client;
      });
    requests.for_each([] (request* rqst)
      {
        delete rqst;
      });
  }

  void addClient(int newSocketFD, sockaddr_in clientAddr)
  {
    std::cout << "ip: " << inet_ntoa(clientAddr.sin_addr) <<
		" port: " << ntohs(clientAddr.sin_port) <<
		" socketFD: " << newSocketFD << std::endl;

    alm::clientstream* client = new alm::clientstream();
    client->openSocket(inet_ntoa(clientAddr.sin_addr), 2100);
    clients.insert(newSocketFD, client);
  }

  void removeClient(int socketFD)
  {
    std::cout << "Closed client socket " << socketFD << std::endl;

    try
    {
      clients.find(socketFD)->closeSocket();
    }
    catch(...){}

    clients.erase(socketFD, [](alm::clientstream* client)
      {
        delete client;
      });
  }

  void recvMessage(int socketFD)
  {
     std::cout << "Received message" << std::endl;
     // TODO: create inmessage on the stack
     // Parse inmessage to create the corresponding task
     // Submit CREATION tasks to the pool
     // Execute STOP tasks from this thread
     if(msg)
     {
       delete msg;
     }
     msg = new alm::inmessage();

     alm::network::recvMessage(socketFD, *msg);
     // If the message is read from another thread, it generates
     // a segmentation fault.
     pool.submit( [&, socketFD]
        {
          worker(socketFD, *msg);
        });
  }

  void worker(int socketFD, alm::inmessage &msg)
  {
    std::cout << "threadID: " << std::this_thread::get_id() << " socketFD: " << socketFD << std::endl;
    std::cout.write((const char*)msg.data, msg.size);
    std::cout << std::endl;

    alm::ibstream input(msg.size);
    memcpy(input.data(),msg.data,msg.size);
    task_type type;
    input >> type;
    // TODO: move alm_client and alm_server to alm/examples/tasks_processor
    // TODO: Command pattern
    if(type == CREATE)
    {
      static unsigned int ticket = 0;
      {
        std::stringstream ss;
        ss << "Create new task ";
        ss << ticket++;
        std::string ack = ss.str();
        alm::outmessage outmsg;
        outmsg.data = (unsigned char*)ack.c_str();
        outmsg.size = ack.length();
        try
        {
          clients.find(socketFD)->sendMessage(outmsg);
        }
        catch(...){}
      }

      request* newrequest = new request();
      requests.insert(ticket, newrequest);

      static unsigned int progress = 0;
      while(newrequest->running)
      {
        std::stringstream ss;
        ss << "Progress: ";
        ss << progress++;
        std::string ack = ss.str();
        alm::outmessage outmsg;
        outmsg.data = (unsigned char*)ack.c_str();
        outmsg.size = ack.length();  
        try
        {
          clients.find(socketFD)->sendMessage(outmsg);
        }
        catch(...){}
        sleep(3);
      }
    }
    else if(type == STOP)
    {
      unsigned int requestID;
      input >> requestID;

      try
      {
        requests.find(requestID)->running = false;
      }
      catch(...){} 
    }
  }
};

int main(void)
{
  processor p;
  alm::serverstream<processor> server(1100, p);
  server.start();

  std::string line;
  while (std::getline(std::cin, line))
  {
    if(line.compare("quit") == 0)
    {
      break;
    }
  }

  server.stop();
}

