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

  char* data();

  unsigned int size();

  template<typename T>
  friend ibstream& operator>> (ibstream &stream, T &field);

private:
  char* m_buffer;

  unsigned int m_size;

  unsigned int m_counter;

  template<typename T>
  void deserialize(T &value)
  {
    unsigned int size = sizeof(value);
    if(m_counter + size > m_size)
    {
      throw out_of_bounds_exception();
    }
    memcpy((char*)&value, m_buffer + m_counter, size);
    m_counter += size;
  }
  
  void deserialize(std::string &field);
  
  void incCounter(unsigned int size);
  
  char currentByte();
};

template<typename T>
ibstream& operator>> (ibstream &stream, T &field)
{
  stream.deserialize(field);
  return stream;
}

}
#endif
