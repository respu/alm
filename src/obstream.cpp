#include <arpa/inet.h>
#include "obstream.h"

namespace alm
{

obstream::obstream()
  : m_buffer(new unsigned char[DEFAULT_CAPACITY]),
    m_capacity(DEFAULT_CAPACITY), m_size(0)
{
}

obstream::~obstream()
{
  delete[] m_buffer;
}

unsigned char* obstream::data()
{
  return m_buffer;
}

unsigned int obstream::size()
{
  return m_size;
}
  
void obstream::serialize(std::string &value)
{
  std::string tmp = value + '\0';
  unsigned int size = sizeof(unsigned char)*tmp.length();
  copyData((void*)tmp.c_str(), size);
}

void obstream::serialize(int value)
{
  uint32_t networkValue = htonl(value);
  copyData(&networkValue, sizeof(networkValue)); 
}

void obstream::serialize(short value)
{
  uint16_t networkValue = htons(value);
  copyData(&networkValue, sizeof(networkValue));
}

void obstream::copyData(void* source, unsigned int size)
{
  if(m_size + size >= m_capacity)
  {
    resize();
  }
    
  memcpy(m_buffer + m_size, source, size);
  m_size += size;
}

void obstream::resize()
{
  m_capacity = m_size * 2;
  unsigned char* new_buffer = new unsigned char[m_capacity];
  memcpy(new_buffer, m_buffer, sizeof(unsigned char) * m_size);

  delete[] m_buffer;
  m_buffer = new_buffer;
}

const unsigned int obstream::DEFAULT_CAPACITY = 128;

}
