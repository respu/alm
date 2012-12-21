#include <string>
#include <sstream>
#include <map>
#include "exceptions.h"
#include "filereader.h"
#include "network.h"
#include "http.h"

namespace alm
{

void http::responseFile(int socketFD, const std::string &fileName)
{
  try
  {
    filereader reader(fileName.c_str());
    responseHeader(socketFD, fileName, reader.size());
    sendFile(socketFD, reader);
  }
  catch(file_not_found_exception &e)
  {
    notFound(socketFD);
  }
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

bool http::allowed(int socketFD, const std::string &url)
{
  bool result = true;

  if(url.find("..") == 0)
  {
    result = false;
    forbidden(socketFD);
  }

  return result;
}

void http::sendFile(int socketFD, alm::filereader &reader)
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
