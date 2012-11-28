#include <iostream>
#include <string>
#include <string.h>
#include "thread_pool.h"
#include "serverstream.h"
#include "http_processor.h"
#include "filereader.h"


class processor
{
public:
  processor() :pool(2) { }

  ~processor() { }

  void request(int socketFD, const std::string &fileName)
  {
    pool.submit([&, socketFD, fileName]
      {
        worker(socketFD, fileName);
      });
  }

  void worker(int socketFD, const std::string &fileName)
  {
    try
    {
      alm::filereader reader(fileName.c_str());
      alm::http_processor<processor>::responseHeader(socketFD, fileName, reader.size());
      response(socketFD, reader);
    }
    catch(alm::file_not_found_exception &e)
    {
      alm::http_processor<processor>::notFound(socketFD);
    }
  }

  void response(int socketFD, alm::filereader &reader)
  {
    unsigned char buffer[reader.size()];
    reader.read(buffer, reader.size());

    int remainingMessageSize = reader.size(); 
    unsigned char* position = buffer;

    int rc;
    while((rc = write(socketFD, position, remainingMessageSize)) > 0)
    {
      remainingMessageSize = remainingMessageSize - rc;
      position += rc;
    } 
  } 

private:
  alm::thread_pool pool;
};

int main(void)
{
  processor p;
  alm::http_processor<processor> http_p(p, "/home/alem/Workspace/web/");
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


