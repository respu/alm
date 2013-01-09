#ifndef __ALM__ENDIANESS__
#define __ALM__ENDIANESS__

namespace alm
{

class little
{
public:
  static unsigned short ushort(unsigned short value);

  static unsigned int uint(unsigned int value);

  static unsigned long long ullong(unsigned long long value);
};

class big
{
public:
  static unsigned short ushort(unsigned short value);

  static unsigned int uint(unsigned int value);

  static unsigned long long ullong(unsigned long long value);
};
}

#endif
