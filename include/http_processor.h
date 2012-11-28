#ifndef __ALM__HTTP_SERVER__
#define __ALM__HTTP_SERVER__

#include <string>
#include <sstream>
#include <map>
#include "exceptions.h"

namespace alm
{

template<typename Processor>
class http_processor
{
public:
  http_processor(Processor &processor, const char* webdir)
    : m_processor(processor), m_webdir(webdir)
  { }

  ~http_processor() { }

  void addClient(int newSocketFD, sockaddr_in clientAddr) { }

  void removeClient(int socketFD) { }

  void recvMessage(int socketFD)
  {
    // If the message is read from another thread, it generates
    // a segmentation fault.
    int rc =read(socketFD, m_input, INPUT_SIZE);
    if ( rc == 0 )
    {
      throw socket_closed_exception();
    }
    else if ( rc == -1 )
    {
      throw socket_error_exception();
    }
    else if( rc > 0)
    {
      std::stringstream ss;
      ss.write(m_input, rc);

      request(socketFD, ss);
    }
  }

  static void responseHeader(int socketFD, const std::string &fileName,
                             unsigned int size)
  {
    std::string ext =
       fileName.substr(fileName.find_last_of('.')+1, fileName.length());
    std::stringstream ss;
    ss << "HTTP/1.1 200 OK\nServer: nweb/\nContent-Length: "
       << size << "\nConnection: close\nContent-Type: "
       << m_extensions[ext] << "\n\n";
    write(socketFD, ss.str().c_str(), ss.str().length()); 
  }

  static void notFound(int socketFD)
  {
    std::stringstream ss;
    ss << "HTTP/1.1 404 Not Found\nContent-Length: 136\nConnection:"
       << " close\nContent-Type: text/html\n\n<html><head>\n"
       << "<title>404 Not Found</title>\n</head><body>\n<h1>Not Found</h1>\n"
       << "The requested URL was not found on this server.\n</body></html>\n";
    write(socketFD, ss.str().c_str(), ss.str().length()); 
  }

  static void forbidden(int socketFD)
  {
    std::stringstream ss;
    ss << "HTTP/1.1 403 Forbidden\nContent-Length: 185\nConnection:"
       << " close\nContent-Type: text/html\n\n<html><head>\n<title>403"
       << " Forbidden</title>\n</head><body>\n<h1>Forbidden</h1>\nThe"
       << " requested URL, file type or operation is not allowed on"
       << " this simple static file webserver.\n</body></html>\n";
    write(socketFD, ss.str().c_str(), ss.str().length());
  }

private:
  static const int INPUT_SIZE = 8096;

  Processor& m_processor;

  std::string m_webdir;

  char m_input[INPUT_SIZE];

  static std::map<std::string, std::string> m_extensions;

  void request(int socketFD, std::stringstream &ss)
  {
    std::string cmd;
    ss >> cmd;
    if(cmd.compare("GET") == 0)
    {
      std::string file;
      ss >> file;

      processRequest(socketFD, file); 
    }
    else
    {
      forbidden(socketFD);
    }
  }

  void processRequest(int socketFD, const std::string &file)
  {
    if(allowed(socketFD, file))
    {
      std::string fileName = m_webdir + file;

      m_processor.request(socketFD, fileName); 
    }
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
};

template<typename Processor>
std::map<std::string, std::string> http_processor<Processor>::m_extensions =
    {
    {"gif", "image/gif"},
    {"jpg", "image/jpg"},
    {"jpeg", "image/jpeg"},
    {"png", "image/png"},
    {"ico", "image/ico"},
    {"html", "text/html"},
    {"js", "text/html"},
    {"css", "text/html"}
    };

}
#endif
