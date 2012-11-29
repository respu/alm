#include <iostream>
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

  void request(int socketFD, const std::string &url)
  {
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
  alm::serverstream<alm::http_processor<processor>> server(1100, http_p);
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


