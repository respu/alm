#include "obstream.h"

namespace alm
{

obstream::obstream()
  : m_buffer(new unsigned char[DEFAULT_CAPACITY]),
    m_capacity(DEFAULT_CAPACITY), m_size(0)
{
}

obstream::obstream(obstream &&other)
  : m_buffer(other.m_buffer), m_capacity(other.m_capacity),
    m_size(other.m_size)
{
  other.m_buffer   = 0;
  other.m_capacity = 0;
  other.m_size     = 0;
}

obstream::~obstream()
{
  clean();
}

unsigned char* obstream::data()
{
  return m_buffer;
}

unsigned long long obstream::size()
{
  return m_size;
}
  
void obstream::serialize(std::string &value)
{
  std::string tmp = value + '\0';
  unsigned long long size = sizeof(unsigned char)*tmp.length();
  write((unsigned char*)tmp.c_str(), size);
}

void obstream::write(unsigned char* data, unsigned long long size)
{
  if(m_size + size >= m_capacity)
  {
    resize(m_size + size);
  }
    
  memcpy(m_buffer + m_size, data, size);
  m_size += size;
}

void obstream::resize(unsigned long long size)
{
  m_capacity = size * 2;
  unsigned char* new_buffer = new unsigned char[m_capacity];
  memcpy(new_buffer, m_buffer, sizeof(unsigned char) * m_size);

  delete[] m_buffer;
  m_buffer = new_buffer;
}

void obstream::clean()
{
  if(m_buffer)
  {
    delete[] m_buffer;
  }
  m_capacity = 0;
  m_size     = 0;
}

}
