#ifndef __HTTP_HANDLER__
#define __HTTP_HANDLER__

#include <string>

class http_handler
{
public:
  http_handler(const char* basedir);

  ~http_handler();

  void process(int socketFD, unsigned char* data,
               unsigned int size);

private:
  std::string base;
};

#endif
