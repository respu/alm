#include "http.h"
#include "http_handler.h"
#include "exceptions.h"

http_handler::http_handler(const char* basedir)
  : base(basedir)
{
}

http_handler::~http_handler()
{
}

void http_handler::process(int socketFD, unsigned char* data,
                           unsigned int size)
{
  alm::http_request request;
  try
  {
    alm::http::parseRequest(socketFD, data, size, request);
  }
  catch(alm::forbidden_exception &e)
  {
    alm::http::forbidden(socketFD);
  }

  std::string fileName = base + request.url;
  try
  {
    alm::http::responseFile(socketFD, fileName);
  }
  catch(alm::file_not_found_exception &e)
  {
    alm::http::notFound(socketFD);
  }
}
