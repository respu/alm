#include <unistd.h>
#include <sys/poll.h>
#include "alm/exceptions.h"
#include "alm/tcp.h"
#include "alm/endianess.h"

namespace alm
{

int tcp::createSocket(unsigned short port, sockaddr_in &sockAddr)
{
  int newSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if(-1 == newSocket)
  {
    throw create_socket_exception(); 
  }

  sockAddr.sin_family = AF_INET;
  sockAddr.sin_port = big::ushort(port);
  sockAddr.sin_addr.s_addr = INADDR_ANY;

  return newSocket;
}

void tcp::bindSocket(int socketFD, sockaddr_in &sockAddr)
{
  if(-1 == bind(socketFD,(sockaddr*)&sockAddr, sizeof(sockAddr)))
  {
    close(socketFD);
    throw bind_socket_exception();
  }
}

void tcp::listenSocket(int socketFD)
{
  if(-1 == listen(socketFD, 10))
  {
    close(socketFD);
    throw listen_socket_exception();
  }
}

void tcp::connectSocket(int socketFD, sockaddr_in &sockAddr)
{
  if (-1 == connect(socketFD, (sockaddr*)&sockAddr, sizeof(sockAddr)))
  {
    close(socketFD);
    throw connect_failed_exception();
  }
}

bool tcp::pollSocket(pollfd* sockets, unsigned int size,
                       unsigned int timeout)
{
  bool result = false;

  int rc = poll(sockets, size, timeout);
  if (rc < 0)
  {
    throw poll_socket_exception();
  }
  else if(rc > 0)
  {
    result = true;
  }

  return result;
}

void tcp::closeSockets(pollfd* sockets, unsigned int size)
{
  for(unsigned int i = 0; i < size; i++)
  {
    if(sockets[i].fd >= 0)
    {
      close(sockets[i].fd);
    }
  }
}

}
