#ifndef __ALM__INPUT_BINARY_STREAM__
#define __ALM__INPUT_BINARY_STREAM__

#include <string>
#include <string.h>

namespace alm
{

struct out_of_bounds_exception : std::exception { };

template<typename endian>
class ibstream
{
public:
  ibstream(unsigned int size)
    : m_buffer(new unsigned char[size]), m_size(size), m_counter(0)
  {
  }

  ~ibstream()
  {
    delete[] m_buffer;
  }

  unsigned char* data()
  {
    return m_buffer;
  }

  unsigned int size()
  {
    return m_size;
  }

  template<typename T>
  ibstream& operator>> (T &field)
  {
    deserialize(field);
    return *this;
  }

private:
  unsigned char* m_buffer;

  unsigned int m_size;

  unsigned int m_counter;

  template<typename T>
  void deserialize(T &value)
  {
    T newValue;
    copyData(&newValue, sizeof(newValue));
    value = endian::translate(newValue);
  }
  
  void deserialize(std::string &value)
  {
    while(currentByte() != '\0')
    {
      value += currentByte();
      incCounter(1);
    }  
    incCounter(1);
  }
 
  void incCounter(unsigned int size)
  {
    if(m_counter + size > m_size)
    {
      throw out_of_bounds_exception();
    } 
    m_counter += size;
  }

  void copyData(void* target, unsigned int size)
  {
    if(m_counter + size > m_size)
    {
      throw out_of_bounds_exception();
    }
    memcpy(target, m_buffer + m_counter, size);
    m_counter += size;
  }
  
  unsigned char currentByte()
  {
    return *(m_buffer + m_counter);
  }
};

}
#endif
