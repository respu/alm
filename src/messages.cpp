#include "messages.h"
#include "exceptions.h"

namespace alm
{

inmessage::inmessage()
  : size(0), data(0)
{
}

inmessage::~inmessage()
{
  if(data)
  {
    delete[] data;
  }
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

}
