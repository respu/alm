#ifndef __ALM__INPUT_BINARY_STREAM__
#define __ALM__INPUT_BINARY_STREAM__

#include <string>
#include <string.h>
#include "exceptions.h"

namespace alm
{

class ibstream
{
public:
  ibstream();

  ibstream(ibstream &&other);

  ~ibstream();
  
  unsigned char* data();
  
  unsigned long long size();
  
  template<typename T>
  ibstream& operator>> (T &field)
  {
    deserialize(field);
    return *this;
  }

  void write(unsigned char* data, unsigned long long length);

  void clean();

  ibstream(const ibstream &other)             = delete;
  ibstream& operator= (const ibstream &other) = delete;

private:
  static const unsigned short DEFAULT_CAPACITY = 128;

  unsigned char* m_buffer;

  unsigned long long m_capacity;

  unsigned long long m_size;

  unsigned long long m_counter;

  template<typename T>
  void deserialize(T &value)
  {
    long long size = sizeof(value);
    if(m_counter + size > m_size)
    {
      throw out_of_bounds_exception();
    }
    memcpy(&value, m_buffer + m_counter, size);
    m_counter += size;
  }
  
  void deserialize(std::string &value);
   
  void incCounter();
  
  unsigned char currentByte();

  void resize(unsigned long long size);

};

}
#endif
