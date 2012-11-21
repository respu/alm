#include "ibstream.h"

namespace alm
{

ibstream::ibstream(unsigned int size)
  : m_buffer(new unsigned char[size]), m_size(size), m_counter(0)
{
}

ibstream::~ibstream()
{
  delete[] m_buffer;
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

void ibstream::copyData(void* target, unsigned int size)
{
  if(m_counter + size > m_size)
  {
    throw out_of_bounds_exception();
  }
  memcpy(target, m_buffer + m_counter, size);
  m_counter += size;
}

}
