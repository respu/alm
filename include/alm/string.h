#ifndef __ALM__STRING__
#define __ALM__STRING__

namespace alm
{

template<typename Allocator = std::allocator<char>>
class string
{
public:
  string(std::size_t length)
    : m_data(0), m_length(length)
  {
    reserve(m_length);
  }

  string(const char* value)
    : m_data(0), m_length(0)
  {
    reserve(strlen(value));

    strcpy(m_data, value);
  }

  string(std::size_t length, Allocator &&allocator)
    : m_data(0), m_length(length), m_allocator(allocator)
  {
    reserve(m_length);
  }

  string(const char* value, Allocator &&allocator)
    : m_data(0), m_length(0), m_allocator(allocator)
  {
    reserve(strlen(value));

    strcpy(m_data, value);
  }

  string(string &&other)
    : m_data(other.m_data), m_length(other.m_length),
      m_allocator(other.m_allocator)
  {
    other.m_data = 0;
    other.m_length = 0;
  }

  ~string()
  {
    if(m_data != 0)
    {
      m_allocator.deallocate(m_data, m_length + 1);
    }
  }

  char* c_str()
  {
    return m_data;
  }

  std::size_t length()
  {
    return m_length;
  }

private:
  char* m_data;

  std::size_t m_length;

  Allocator m_allocator;

  void reserve(std::size_t length)
  {
    m_length = length; 
    m_data = (char*)m_allocator.allocate(m_length + 1);
    // Add \0 character at the end of the string to make
    // it printable by std::cout
    *(m_data + m_length) = '\0';
  }

};

}

#endif

