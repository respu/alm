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

  char* data();

  unsigned int size();

  template<typename T>
  friend obstream& operator<< (obstream &stream, T &field);

private:
  static const unsigned int DEFAULT_CAPACITY;

  char* m_buffer;

  unsigned int m_capacity;

  unsigned int m_size;

  template<typename T>
  void serialize(T &value)
  {
    unsigned int size = sizeof(T);

    if(m_size + size >= m_capacity)
    {
      resize();
    }
    
    memcpy(m_buffer + m_size, (char*)&value, size);
    m_size += size;
  }
  
  void serialize(std::string &field);

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
