#ifndef __ALM__TCP__
#define __ALM__TCP__

#include <arpa/inet.h>
#include <sys/poll.h>

namespace alm
{

class tcp
{
public:
  static const int MAX_SOCKETS = 200;

  static int createSocket(unsigned short port, sockaddr_in &sockAddr);

  static void bindSocket(int socketFD, sockaddr_in &sockAddr);

  static void listenSocket(int socketFD);
  
  static void connectSocket(int socketFD, sockaddr_in &sockAddr);
  
  static bool pollSocket(pollfd* sockets, unsigned int size,
                         unsigned int timeout);
  
  static void closeSockets(pollfd* sockets, unsigned int size);
};

}

#endif
