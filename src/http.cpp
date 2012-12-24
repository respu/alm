#include <string>
#include <sstream>
#include <map>
#include "exceptions.h"
#include "filereader.h"
#include "network.h"
#include "http.h"

#include <iostream>

namespace alm
{

void http::parseRequest(int socketFD, unsigned char* data,
                 unsigned int size, http_request &request)
{
  std::stringstream ss;
  ss.write((const char*)data, size);

  std::string cmd;
  ss >> cmd >> request.url;

  if(request.url.find("..") == 0)
  {
    throw forbidden_exception();
  }

  parseParameters(socketFD, cmd, ss, request);
}

void http::parseParameters(int socketFD, std::string &cmd,
                    std::stringstream &ss, http_request &request)
{
  if(cmd.compare("GET") == 0)
  {
    parseGet(socketFD, ss, request);
  }
  else if(cmd.compare("POST") == 0)
  {
    parsePost(socketFD, ss, request);
  }
  else
  {
    throw forbidden_exception();
  }
}

void http::parseGet(int socketFD, std::stringstream &ss,
                    http_request &request)
{
  std::cout << "GET" << std::endl;
  std::cout << ss.str() << std::endl;
}

void http::parsePost(int socketFD, std::stringstream &ss,
                     http_request &request)
{
  std::cout << "POST" << std::endl;
  std::cout << ss.str() << std::endl;
}

void http::responseFile(int socketFD, const std::string &fileName)
{
  filereader reader(fileName.c_str());
  
  responseHeader(socketFD, fileName, reader.size());
  
  unsigned char buffer[reader.size()];
  reader.read(buffer, reader.size());
  
  network::writeAllData(socketFD, buffer, reader.size());
}

void http::responseHeader(int socketFD, const std::string &fileName,
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

void http::notFound(int socketFD)
{
  std::stringstream ss;
  ss << "HTTP/1.1 404 Not Found\nContent-Length: 136\nConnection:"
     << " close\nContent-Type: text/html\n\n<html><head>\n"
     << "<title>404 Not Found</title>\n</head><body>\n<h1>Not Found</h1>\n"
     << "The requested URL was not found on this server.\n</body></html>\n";
  write(socketFD, ss.str().c_str(), ss.str().length()); 
}

void http::forbidden(int socketFD)
{
  std::stringstream ss;
  ss << "HTTP/1.1 403 Forbidden\nContent-Length: 185\nConnection:"
     << " close\nContent-Type: text/html\n\n<html><head>\n<title>403"
     << " Forbidden</title>\n</head><body>\n<h1>Forbidden</h1>\nThe"
     << " requested URL, file type or operation is not allowed on"
     << " this simple file webserver.\n</body></html>\n";
  write(socketFD, ss.str().c_str(), ss.str().length());
}

std::map<std::string, std::string> http::m_extensions =
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
