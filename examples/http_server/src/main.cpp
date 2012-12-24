#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include "thread_pool.h"
#include "tcp_server.h"
#include "http.h"
#include "network.h"

class http_handler
{
public:
  http_handler(const char* basedir) :pool(2), base(basedir) { }

  ~http_handler() { }

  void process(int socketFD, unsigned char* data, unsigned int size)
  {
    alm::http_request request;
    try
    {
      alm::http::parseRequest(socketFD, data, size, request);
    }
    catch(alm::forbidden_exception &e)
    {
      alm::http::forbidden(socketFD);
    }

    std::string fileName = base + request.url;
    pool.submit([socketFD, fileName]
      {
        try
        {
          alm::http::responseFile(socketFD, fileName);
        }
        catch(alm::file_not_found_exception &e)
        {
          alm::http::notFound(socketFD);
        }
      });
  }

private:
  alm::thread_pool pool;

  std::string base;
};

class http_processor
{
public:
  http_processor()
    : m_handler("/home/alem/Workspace/web/")
  {
  }

  void addClient(int newSocketFD, sockaddr_in clientAddr) { }

  void removeClient(int socketFD) { }

  void recvMessage(int socketFD)
  {
    int rc = alm::network::readData(socketFD, m_buffer, BLOCK);
    if( rc > 0)
    {
      m_handler.process(socketFD, m_buffer, rc);
    }
  }

private:
  static const short BLOCK = 16384;

  unsigned char m_buffer[BLOCK];

  http_handler m_handler;
};


int main(void)
{
  http_processor processor;
  alm::tcp_server<http_processor> server;
  server.start(1100, processor, 5000);

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


