#include "ibstream.h"

namespace alm
{

ibstream::ibstream(unsigned int size)
  : m_buffer(new char[size]), m_size(size), m_counter(0)
{
}

ibstream::~ibstream()
{
  delete[] m_buffer;
}

char* ibstream::data()
{
  return m_buffer;
}

unsigned int ibstream::size()
{
  return m_size;
}
  
void ibstream::deserialize(std::string &field)
{
  while(currentByte() != '\0')
  {
    field += currentByte();
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

char ibstream::currentByte()
{
  return *(m_buffer + m_counter);
}

}
