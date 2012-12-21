#include <iostream>
#include <string>
#include <string.h>
#include <map>
#include <sstream>
#include <atomic>
#include "tcp_client.h"
#include "tcp_server.h"
#include "network.h"
#include "thread_pool.h"
#include "safe_map.h"
#include "ibstream.h"
#include "endianess.h"

enum task_type { CREATE, PAUSE, RESUME, STOP};

struct request
{
  std::atomic<bool> running;

  unsigned int ticket;

  request(unsigned int t):running(true), ticket(t){}
};

class processor
{
public:
  alm::thread_pool pool;
  alm::safe_map<unsigned int, request*> requests;

  processor():pool(2){}

  ~processor()
  {
    requests.for_each([] (request* rqst)
      {
        delete rqst;
      });
  }

  void addClient(int newSocketFD, sockaddr_in clientAddr)
  {
    std::cout << "ip: " << inet_ntoa(clientAddr.sin_addr) <<
		" port: " << alm::little::ushort(clientAddr.sin_port) <<
		" socketFD: " << newSocketFD << std::endl;
  }

  void removeClient(int socketFD)
  {
    std::cout << "Closed client socket " << socketFD << std::endl;
  }

  void recvMessage(int socketFD)
  {
     std::cout << "Received message" << std::endl;
     
     alm::message msg;

     // If the message is read from another thread, it generates
     // a segmentation fault.
     alm::protocol::recv(socketFD, msg);

     processMessage(socketFD, msg);
  }

  void processMessage(int socketFD, alm::message &msg)
  {
    alm::ibstream input;
    input.write(msg.data,msg.size);
    unsigned int networkType;
    input >> networkType; 
    task_type type = (task_type)alm::little::uint(networkType);
    if(type == CREATE)
    {
      createRequest(socketFD);
    }
    else if(type == STOP)
    {
      stopRequest(input);
    }
  }

  void createRequest(int socketFD)
  {
      static unsigned int ticket = 0;
      
      request* newrequest = new request(ticket);
      requests.insert(ticket, newrequest);

      notifyClient(socketFD, ticket);

      pool.submit([&, socketFD, newrequest]
        {
          worker(socketFD, *newrequest);
        });
      
      ticket++;
  }

  void notifyClient(int socketFD, unsigned int ticket)
  {
     std::stringstream ss;
     ss << "Created new task: " << ticket;
     std::string ack = ss.str();
     alm::protocol::send(socketFD, (unsigned char*)ack.c_str(), ack.length());
  }

  void stopRequest(alm::ibstream &input)
  {
    int32_t requestID;
    input >> requestID;
    unsigned int id = alm::little::uint(requestID);
    try
    {
      requests.find(id)->running = false;
    }
    catch(...){}
  }

  void worker(int socketFD, request &rqst)
  {
    std::cout << "threadID: " << std::this_thread::get_id() << std::endl;
    unsigned int progress = 0;
    while(rqst.running)
    {
      std::stringstream ss;
      ss << "Task: " << rqst.ticket << " Progress: " << progress++;
      std::string ack = ss.str();
      alm::protocol::send(socketFD, (unsigned char*)ack.c_str(), ack.length());

      sleep(3);
    }
  }
};

int main(void)
{
  processor p;
  alm::tcp_server<processor> server;
  server.start(1100, p, 5000);

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

