#ifndef __ALM__MESSAGES__
#define __ALM__MESSAGES__

namespace alm
{

struct outmessage
{
  unsigned int size;
  unsigned char* data;
};

struct inmessage
{
  unsigned int size;
  unsigned char* data;

  inmessage();

  ~inmessage();

  void allocate(unsigned int msgSize);
};

}

#endif
