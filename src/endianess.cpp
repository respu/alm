#include <arpa/inet.h>
#include <stdint.h>
#include "alm/endianess.h"

namespace alm
{

unsigned short little::ushort(unsigned short value)
{
  return ntohs(value);
}

unsigned int little::uint(unsigned int value)
{
  return ntohl(value);
}

unsigned long long little::ullong(unsigned long long value)
{
  return be64toh(value);
}

unsigned short big::ushort(unsigned short value)
{
  return htons(value);
}

unsigned int big::uint(unsigned int value)
{
  return htonl(value);
}

unsigned long long big::ullong(unsigned long long value)
{
  return htobe64(value);
}

}
