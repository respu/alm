#include <iostream>
#include <string.h>
#include <iostream>
#include <sstream>
#include "clientstream.h"
#include "serverstream.h"
#include "network.h"
#include "obstream.h"
#include "ibstream.h"

enum task_type { CREATE, PAUSE, RESUME, STOP};

struct base_task
{
  task_type type;
  base_task(task_type t):type(t){}
};

struct create_task : base_task
{
  create_task():base_task(CREATE){}

  void serialize(alm::obstream &output)
  {
    output << type;
  }
};

struct stop_task : base_task
{
  unsigned int requestID;

  stop_task():base_task(STOP){}

  void serialize(alm::obstream &output)
  {
    output << type << requestID;
  }
};

struct processor
{
  void addClient(int newSocketFD, sockaddr_in clientAddr)
  {
    std::cout << "ip: " << inet_ntoa(clientAddr.sin_addr) <<
		" port: " << ntohs(clientAddr.sin_port) <<
		" socketFD: " << newSocketFD << std::endl;
  }

  void removeClient(int socketFD)
  {
    std::cout << "Closed client socket " << socketFD << std::endl;
  }

  void recvMessage(int socketFD)
  {
     std::cout << "Received Message" << std::endl;
     alm::inmessage msg;
     alm::network::recvMessage(socketFD, msg);
     std::cout.write((const char*)msg.data, msg.size);
     std::cout << std::endl;
  }
};

void createTask(alm::clientstream &client)
{
  create_task task;

  alm::obstream output;
  task.serialize(output);
      
  alm::outmessage outmsg;
  outmsg.data = (unsigned char*)output.data();
  outmsg.size = output.size();
  client.sendMessage(outmsg);
}

int main()
{
  processor p;
  alm::serverstream<processor> server(2100, p);
  server.start();

  alm::clientstream client;
  client.openSocket("127.0.0.1", 1100);

  std::string line;
  while (std::getline(std::cin, line))
  {
    std::stringstream ss(line);
    std::string command;
    ss >> command;

    if(command.compare("quit") == 0)
    {
      break;
    }
    else if(command.compare("create") == 0)
    {
      createTask(client);
    }
    else if(command.compare("stop") == 0)
    {
      stop_task task;
      ss >> task.requestID;

      alm::obstream output;
      task.serialize(output);
      
      alm::outmessage outmsg;
      outmsg.data = (unsigned char*)output.data();
      outmsg.size = output.size();
      client.sendMessage(outmsg); 
    }
  }

  client.closeSocket();

  server.stop();
}

