#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <map>
#include "serverstream.h"
#include "network.h"
#include "thread_pool.h"
#include "filereader.h"
#include "exceptions.h"

class processor
{
public:
  processor()
    :pool(2)
  {
    web_base = "/home/alem/Workspace/web/";

    extensions["gif"]  = "image/gif";
    extensions["jpg"]  = "image/jpg";
    extensions["jpeg"] = "image/jpeg";
    extensions["png"]  = "image/png";
    extensions["ico"]  = "image/ico";
    extensions["html"] = "text/html";
    extensions["js"]   = "text/html";
    extensions["css"]  = "text/html";
  }

  ~processor() { }

  void addClient(int newSocketFD, sockaddr_in clientAddr) { }

  void removeClient(int socketFD) { }

  void recvMessage(int socketFD)
  {
    // If the message is read from another thread, it generates
    // a segmentation fault.
    int rc =read(socketFD, input, INPUT_SIZE);
    if ( rc == 0 )
    {
      close(socketFD);
      throw alm::socket_closed_exception();
    }
    else if ( rc == -1 )
    {
      close(socketFD);
      throw alm::socket_error_exception();
    }
    else if( rc > 0)
    {
      std::stringstream ss;
      ss.write(input, rc);

      processMessage(socketFD, ss);
    }
  }

  void processMessage(int socketFD, std::stringstream &ss)
  {
    std::string cmd;
    ss >> cmd;
    if(cmd.compare("GET") == 0)
    {
      std::string file;
      ss >> file;
      pool.submit([&, socketFD, file]
        {
          worker(socketFD, file);
        });
    }
    else
    {
      forbidden(socketFD);
    }
  }

  void worker(int socketFD, const std::string &file)
  {
    if(allowed(socketFD, file))
    {
      try
      {
        std::string fileName = web_base + file;
        alm::filereader reader(fileName.c_str());
        responseHeader(socketFD, fileName, reader.size());
        response(socketFD, reader);
      }
      catch(alm::file_not_found_exception &e)
      {
        notFound(socketFD);
      }
    }
  }

  void responseHeader(int socketFD, std::string &fileName, int size)
  {
    std::string ext = fileName.substr(fileName.find_last_of('.')+1, fileName.length());
    std::stringstream ss;
    ss << "HTTP/1.1 200 OK\nServer: nweb/\nContent-Length: "
       << size << "\nConnection: close\nContent-Type: "
       << extensions[ext] << "\n\n";
    write(socketFD, ss.str().c_str(), ss.str().length()); 
  }

  void notFound(int socketFD)
  {
    std::stringstream ss;
    ss << "HTTP/1.1 404 Not Found\nContent-Length: 136\nConnection:"
       << " close\nContent-Type: text/html\n\n<html><head>\n"
       << "<title>404 Not Found</title>\n</head><body>\n<h1>Not Found</h1>\n"
       << "The requested URL was not found on this server.\n</body></html>\n";
    write(socketFD, ss.str().c_str(), ss.str().length()); 
  }

  void forbidden(int socketFD)
  {
    std::stringstream ss;
    ss << "HTTP/1.1 403 Forbidden\nContent-Length: 185\nConnection:"
       << " close\nContent-Type: text/html\n\n<html><head>\n<title>403"
       << " Forbidden</title>\n</head><body>\n<h1>Forbidden</h1>\nThe"
       << " requested URL, file type or operation is not allowed on"
       << " this simple static file webserver.\n</body></html>\n";
    write(socketFD, ss.str().c_str(), ss.str().length());
  }

  bool allowed(int socketFD, const std::string &file)
  {
    bool result = true;

    if(file.find("..") == 0)
    {
      result = false;
      forbidden(socketFD);
    }

    return result;
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
  static const int INPUT_SIZE = 8096;

  std::string web_base;

  char input[INPUT_SIZE];

  alm::thread_pool pool;

  std::map<std::string, std::string> extensions;

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


