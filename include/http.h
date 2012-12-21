#ifndef __ALM__HTTP__
#define __ALM__HTTP__

#include <string>
#include <sstream>
#include <map>
#include "filereader.h"

namespace alm
{

class http
{
public:
  template<typename handler>
  static void request(int socketFD, unsigned char* data,
                      unsigned int size, handler &_handler)
  {
    std::stringstream ss;
    ss.write((const char*)data, size);

    std::string cmd;
    std::string url;
    ss >> cmd >> url;

    if(allowed(socketFD, url))
    {
      processRequest(socketFD, cmd, url, ss, _handler);
    }
  }

  static void responseFile(int socketFD, const std::string &fileName);
  
private:
  static std::map<std::string, std::string> m_extensions;

  template<typename handler>
  static void processRequest(int socketFD, std::string &cmd,
                             std::string &url, std::stringstream &ss,
                             handler &_handler)
  {
    if(cmd.compare("GET") == 0)
    {
      _handler.doGet(socketFD, url);
    }
    else if(cmd.compare("POST") == 0)
    {
      std::string tmp = ss.str();
      std::string message = 
        tmp.substr(tmp.find_last_of("\n") + 1, tmp.length());
      _handler.doPost(socketFD, url, message);
    }
    else
    {
      forbidden(socketFD);
    }
  }

  static void responseHeader(int socketFD, const std::string &fileName,
                             unsigned int size);
  
  static void notFound(int socketFD);
  
  static void forbidden(int socketFD);

  static bool allowed(int socketFD, const std::string &url);
  
  static void sendFile(int socketFD, alm::filereader &reader);
};

}
#endif
