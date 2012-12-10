#include "ibstream.h"

namespace alm
{

ibstream::ibstream()
  : m_buffer(new unsigned char[DEFAULT_CAPACITY]),
    m_capacity(DEFAULT_CAPACITY), m_size(0), m_counter(0)
{
}

ibstream::ibstream(ibstream &&other)
  : m_buffer(other.m_buffer), m_capacity(other.m_capacity),
    m_size(other.m_size), m_counter(other.m_counter)
{
  other.m_buffer   = 0;
  other.m_capacity = 0;
  other.m_size     = 0;
  other.m_counter  = 0;
}

ibstream::~ibstream()
{
  clean();
}

unsigned char* ibstream::data()
{
  return m_buffer;
}

unsigned int ibstream::size()
{
  return m_size;
}
  
void ibstream::deserialize(std::string &value)
{
  while(currentByte() != '\0')
  {
    value += currentByte();
    incCounter(1);
  }  
  incCounter(1);
}

void ibstream::incCounter(unsigned int size)
{
  if(m_counter + size > m_size)
  {
    throw out_of_bounds_exception();
  } 
  m_counter += size;
}

unsigned char ibstream::currentByte()
{
  return *(m_buffer + m_counter);
}

void ibstream::write(unsigned char* data, unsigned int size)
{
  if(m_size + size >= m_capacity)
  {
    resize(m_size + size);
  }
    
  memcpy(m_buffer + m_size, data, size);
  m_size += size;
}

void ibstream::resize(unsigned int size)
{
  m_capacity = size * 2;
  unsigned char* new_buffer = new unsigned char[m_capacity];
  memcpy(new_buffer, m_buffer, sizeof(unsigned char) * m_size);

  delete[] m_buffer;
  m_buffer = new_buffer;
}

void ibstream::clean()
{
  if(m_buffer)
  {
    delete[] m_buffer;
  }
  m_capacity = 0;
  m_size     = 0;
  m_counter  = 0;
}

}
