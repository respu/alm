#ifndef __ALM__OUTPUT_BINARY_STREAM__
#define __ALM__OUTPUT_BINARY_STREAM__

#include <string>
#include <string.h>

namespace alm
{

template<typename endian>
class obstream
{
public:
  obstream()
    : m_buffer(new unsigned char[DEFAULT_CAPACITY]),
      m_capacity(DEFAULT_CAPACITY), m_size(0)
  {
  }

  ~obstream()
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
  obstream& operator<< (T &field)
  {
    serialize(field);
    return *this;
  }

private:
  static const unsigned int DEFAULT_CAPACITY;

  unsigned char* m_buffer;

  unsigned int m_capacity;

  unsigned int m_size;

  template<typename T>
  void serialize(T &value)
  {
    T newValue = endian::translate(value);
    copyData(&newValue, sizeof(newValue));
  }

  void serialize(std::string &value)
  {
    std::string tmp = value + '\0';
    unsigned int size = sizeof(unsigned char)*tmp.length();
    copyData((void*)tmp.c_str(), size);
  }

  void copyData(void* source, unsigned int size)
  {
    if(m_size + size >= m_capacity)
    {
      resize();
    }
    
    memcpy(m_buffer + m_size, source, size);
    m_size += size;
  }

  void resize()
  {
    m_capacity = m_size * 2;
    unsigned char* new_buffer = new unsigned char[m_capacity];
    memcpy(new_buffer, m_buffer, sizeof(unsigned char) * m_size);

    delete[] m_buffer;
    m_buffer = new_buffer;
  }
};

template<typename endian>
const unsigned int obstream<endian>::DEFAULT_CAPACITY = 128;
/**
template<typename T, typename endian>
obstream<endian>& operator<< (obstream<endian> &stream, T &field)
{
  stream.serialize(field);
  return stream;
}
*/
}
#endif
