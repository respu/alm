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

  void doGet(int socketFD, const std::string &url)
  {
    std::cout << "URL: " << url << std::endl;

    std::string fileName = base + url;
    pool.submit([&, socketFD, fileName]
      {
        alm::http::responseFile(socketFD, fileName);
      });
  }

  void doPost(int socketFD, const std::string &url, const std::string &message)
  {
    std::cout << "URL: " << url << std::endl;
    std::cout << "Message: " << message << std::endl;

    std::string fileName = base + url;
    pool.submit([&, socketFD, fileName]
      {
        alm::http::responseFile(socketFD, fileName);
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
      alm::http::request(socketFD, m_buffer, rc, m_handler);
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


