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

  obstream(obstream &&other);

  ~obstream();

  unsigned char* data();

  unsigned long long size();

  template<typename T>
  obstream& operator<< (T &field)
  {
    serialize(field);
    return *this;
  }

  void write(unsigned char* data, unsigned long long length);

  void clean();

  obstream(const obstream &other)             = delete;
  obstream& operator= (const obstream &other) = delete;

private:
  static const unsigned short DEFAULT_CAPACITY = 128;

  unsigned char* m_buffer;

  unsigned long long m_capacity;

  unsigned long long m_size;

  template<typename T>
  void serialize(T &value)
  {
    write((unsigned char*)&value, sizeof(value));
  }

  void serialize(std::string &value);

  void resize(unsigned long long size);
};

}
#endif
