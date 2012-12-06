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
  static const unsigned int DEFAULT_CAPACITY = 128;

  unsigned char* data;

  unsigned int capacity;
  
  unsigned int size;

  inmessage();

  inmessage(inmessage &&other);

  ~inmessage();

  void allocate(unsigned int msgSize);

  void write(unsigned char* source, unsigned int length);

  void resize(int length);

  void clean();
};

}

#endif
