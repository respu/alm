#ifndef __ALM__NETWORK__
#define __ALM__NETWORK__

namespace alm
{

struct inmessage
{
  unsigned int size;
  unsigned char* data;

  inmessage();

  ~inmessage();

  void allocate(unsigned int msgSize);
};

class network
{
public:
  static void recv(int socketFD, inmessage &msg);

  static void send(int socketFD, unsigned char* data, unsigned int size); 

private:
  static const int HEADER_SIZE; 

  static unsigned int readHeader(int socketFD);

  static void readBody(int socketFD, inmessage &msg, unsigned int totalSize);
};

}

#endif
