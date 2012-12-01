#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include "thread_pool.h"
#include "serverstream.h"
#include "http_processor.h"

class processor
{
public:
  processor(const char* basedir) :pool(2), base(basedir) { }

  ~processor() { }

  void doGet(int socketFD, const std::string &url)
  {
    std::cout << "URL: " << url << std::endl;

    std::string fileName = base + url;
    pool.submit([&, socketFD, fileName]
      {
        alm::http_processor<processor>::responseFile(socketFD, fileName);
      });
  }

  void doPost(int socketFD, const std::string &url, const std::string &message)
  {
    std::cout << "URL: " << url << std::endl;
    std::cout << "Message: " << message << std::endl;

    std::string fileName = base + url;
    pool.submit([&, socketFD, fileName]
      {
        alm::http_processor<processor>::responseFile(socketFD, fileName);
      });
  }

private:
  alm::thread_pool pool;

  std::string base;
};

int main(void)
{
  processor p("/home/alem/Workspace/web/");
  alm::http_processor<processor> http_p(p);
  alm::serverstream<alm::http_processor<processor>> server;
  server.start(110, http_p, 5000);

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


