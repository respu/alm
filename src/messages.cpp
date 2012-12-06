#include <string.h>
#include "messages.h"
#include "exceptions.h"

namespace alm
{

inmessage::inmessage()
  : data(new unsigned char[DEFAULT_CAPACITY]),
    capacity(DEFAULT_CAPACITY), size(0)
{
}

inmessage::inmessage(inmessage &&other)
  : data(other.data), capacity(other.capacity),
    size(other.size)
{
  other.data = 0;
  other.capacity = 0;
  other.size = 0;
}


inmessage::~inmessage()
{
  clean();
}

void inmessage::allocate(unsigned int msgSize)
{
  if(data)
  {
    throw message_allocation_exception();
  }
  size = msgSize;
  data = new unsigned char[size];
}

void inmessage::write(unsigned char* source, unsigned int length)
{
  if(size + length >= capacity)
  {
    resize(length);
  }
    
  memcpy(data + size, source, length);
  size += length;
}

void inmessage::resize(int length)
{
  capacity = (size + length) * 2;
  unsigned char* new_data = new unsigned char[capacity];
  memcpy(new_data, data, sizeof(unsigned char) * size);

  delete[] data;
  data = new_data;
}

void inmessage::clean()
{
  if(data)
  {
    delete[] data;
  }
  capacity = 0;
  size = 0;
}

}
