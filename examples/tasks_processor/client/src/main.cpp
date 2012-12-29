#include <iostream>
#include <string.h>
#include <iostream>
#include <sstream>
#include "alm/tcp_client.h"
#include "alm/network.h"
#include "alm/obstream.h"
#include "alm/endianess.h"

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
    unsigned int networkType = alm::big::uint(type);
    output << networkType;
  }
};

struct stop_task : base_task
{
  unsigned int requestID;

  stop_task():base_task(STOP){}

  void serialize(alm::obstream &output)
  {
    unsigned int networkType = alm::big::uint(type);
    unsigned int networkRequestID = alm::big::uint(requestID);

    output << networkType << networkRequestID;
  }
};

struct processor
{
  void recvMessage(int socketFD)
  {
     std::cout << "Received Message" << std::endl;
     alm::message msg;
     alm::protocol::recv(socketFD, msg);
     std::cout.write((const char*)msg.data, msg.size);
     std::cout << std::endl;
  }

  void sendMessage(int socketFD, unsigned char* data,
                   unsigned int size)
  {
    alm::protocol::send(socketFD, data, size);
  }
};

template<typename Client>
void createTask(Client &client)
{
  create_task task;

  alm::obstream output;
  task.serialize(output);
      
  client.sendMessage(output.data(), output.size());
}

template<typename Client>
void stopTask(Client &client, int requestID)
{
  stop_task task;
  task.requestID = requestID;

  alm::obstream output;
  task.serialize(output);
     
  client.sendMessage(output.data(), output.size());
}

int main()
{
  processor p;

  alm::tcp_client<processor> client;
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

