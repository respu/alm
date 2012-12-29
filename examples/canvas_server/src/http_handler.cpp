#include "alm/http.h"
#include "alm/exceptions.h"
#include "http_handler.h"

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
    alm::http::readRequest(socketFD, data, size, request);
  }
  catch(alm::forbidden_exception &e)
  {
    alm::http::forbidden(socketFD);
  }

  std::string fileName = base + request.url;
  try
  {
    alm::http::writeFile(socketFD, fileName);
  }
  catch(alm::file_not_found_exception &e)
  {
    alm::http::notFound(socketFD);
  }
}
