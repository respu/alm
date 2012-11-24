#ifndef __ALM__ENDIANESS__
#define __ALM__ENDIANESS__

namespace alm
{

struct big
{
  static unsigned int translate(unsigned int value);
};

struct little
{
  static unsigned int translate(unsigned int value);
};

struct same
{
  template<typename T>
  static T translate(T value)
  {
    return value;
  }
};

}

#endif
