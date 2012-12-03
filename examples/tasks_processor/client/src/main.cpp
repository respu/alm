#include <iostream>
#include <string.h>
#include <iostream>
#include <sstream>
#include <arpa/inet.h>
#include "clientstream.h"
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
    int32_t networkType = htonl(type);
    output << networkType;
  }
};

struct stop_task : base_task
{
  unsigned int requestID;

  stop_task():base_task(STOP){}

  void serialize(alm::obstream &output)
  {
    int32_t networkType = htonl(type);
    uint32_t networkRequestID = htonl(requestID);

    output << networkType << networkRequestID;
  }
};

struct processor
{
  void recvMessage(int socketFD)
  {
     std::cout << "Received Message" << std::endl;
     alm::inmessage msg;
     alm::network::recvMessage(socketFD, msg);
     std::cout.write((const char*)msg.data, msg.size);
     std::cout << std::endl;
  }
};

template<typename Client>
void createTask(Client &client)
{
  create_task task;

  alm::obstream output;
  task.serialize(output);
      
  alm::outmessage outmsg;
  outmsg.data = output.data();
  outmsg.size = output.size();
  client.sendMessage(outmsg);
}

template<typename Client>
void stopTask(Client &client, int requestID)
{
  stop_task task;
  task.requestID = requestID;

  alm::obstream output;
  task.serialize(output);
     
  alm::outmessage outmsg;
  outmsg.data = output.data();
  outmsg.size = output.size();
  client.sendMessage(outmsg);
}

int main()
{
  processor p;

  alm::clientstream<processor> client;
  client.start("127.0.0.1", 1100, p, 5000);

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
      int requestID;
      ss >> requestID;
      stopTask(client, requestID);
    }
  }

  client.stop();
}

