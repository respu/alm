#ifndef __ALM__HTTP__
#define __ALM__HTTP__

#include <string>
#include <sstream>
#include <map>
#include "filereader.h"

namespace alm
{

struct http_request
{
  std::string                        url;
  std::map<std::string, std::string> parameters;
};

class http
{
public:
  static void parseRequest(int socketFD, unsigned char* data,
                      unsigned int size, http_request &request);

  static void responseFile(int socketFD, const std::string &fileName);

  static void notFound(int socketFD);
  
  static void forbidden(int socketFD); 

private:
  static std::map<std::string, std::string> m_extensions;

  static void parseParameters(int socketFD, std::string &cmd,
                             std::stringstream &ss, http_request &request);

  static void parseGet(int socketFD, std::stringstream &ss, http_request &request);

  static void parsePost(int socketFD, std::stringstream &ss, http_request &request);
  
  static void responseHeader(int socketFD, const std::string &fileName,
                             unsigned int size);
};

}
#endif
