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
  friend obstream& operator<< (obstream &stream, T &field);

private:
  static const unsigned int DEFAULT_CAPACITY;

  unsigned char* m_buffer;

  unsigned int m_capacity;

  unsigned int m_size;

  template<typename T>
  void serialize(T &value)
  {
    copyData(&value, sizeof(T));
  }
  
  void serialize(std::string &value);

  void copyData(void* source, unsigned int size);

  void resize();
};

template<typename T>
obstream& operator<< (obstream &stream, T &field)
{
  stream.serialize(field);
  return stream;
}

}
#endif
