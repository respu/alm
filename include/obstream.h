#ifndef __ALM__OUTPUT_BINARY_STREAM__
#define __ALM__OUTPUT_BINARY_STREAM__

#include <string>
#include <string.h>

namespace alm
{

class obstream
{
public:
  obstream();

  ~obstream();

  unsigned char* data();

  unsigned int size();

  template<typename T>
  obstream& operator<< (T &field)
  {
    serialize(field);
    return *this;
  }

private:
  static const unsigned int DEFAULT_CAPACITY = 128;

  unsigned char* m_buffer;

  unsigned int m_capacity;

  unsigned int m_size;

  template<typename T>
  void serialize(T &value)
  {
    copyData(&value, sizeof(value));
  }

  void serialize(std::string &value);

  void copyData(void* source, unsigned int size);

  void resize();
};

}
#endif
