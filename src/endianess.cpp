#include <arpa/inet.h>
#include "endianess.h"

namespace alm
{

unsigned int big::translate(unsigned int value)
{
  return htonl(value);
}

unsigned int little::translate(unsigned int value)
{
  return ntohl(value);
}
}
