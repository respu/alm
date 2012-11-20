#ifndef __ALM__INPUT_BINARY_STREAM__
#define __ALM__INPUT_BINARY_STREAM__

#include <string>
#include <string.h>

namespace alm
{

struct out_of_bounds_exception : std::exception { };

class ibstream
{
public:
  ibstream(unsigned int size);

  ~ibstream();

  unsigned char* data();

  unsigned int size();

  template<typename T>
  friend ibstream& operator>> (ibstream &stream, T &field);

private:
  unsigned char* m_buffer;

  unsigned int m_size;

  unsigned int m_counter;

  template<typename T>
  void deserialize(T &value)
  {
    copyData(&value, sizeof(value));
  }
  
  void deserialize(std::string &value);

  void deserialize(int value);

  void deserialize(short value);
  
  void incCounter(unsigned int size);

  void copyData(void* target, unsigned int size);
  
  unsigned char currentByte();
};

template<typename T>
ibstream& operator>> (ibstream &stream, T &field)
{
  stream.deserialize(field);
  return stream;
}

}
#endif
