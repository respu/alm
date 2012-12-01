#ifndef __ALM__BASE64__
#define __ALM__BASE64__

#include <stdexcept>
#include <string>

namespace alm
{

class base64
{
public:
  static const std::string decode(const std::string& src);

  static const std::string encode(const std::string& src);

private:
  static const char table[];

  static const char pad;

  static const short reverse_table[256];
};

}

#endif
