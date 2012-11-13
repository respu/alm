#ifndef __ALM__NETWORK__
#define __ALM__NETWORK__

#include "messages.h"

namespace alm
{

class network
{
public:
  static void recvMessage(int socketFD, inmessage &msg);

  static void sendMessage(int socketFD, outmessage &msg); 

private:
  static const int HEADER_SIZE; 

  static unsigned int readHeader(int socketFD);

  static void readBody(int socketFD, inmessage &msg, unsigned int totalSize);
};

}

#endif
