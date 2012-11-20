#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "network.h"
#include "exceptions.h"

namespace alm
{

const int network::HEADER_SIZE = sizeof(int);

unsigned int network::readHeader(int socketFD)
{
  unsigned int messageTotalSize = 0;

  unsigned char headerBuf[HEADER_SIZE];

  int rc = recv(socketFD, headerBuf, HEADER_SIZE , 0);

  if ( rc == 0 )
  {
    close(socketFD);
    throw socket_closed_exception();
  }
  else if ( rc == -1 )
  {
    close(socketFD);
    throw socket_error_exception();
  }
  else if ( rc != HEADER_SIZE )
  {
    close(socketFD);
    throw read_header_exception();
  }
   
  memcpy(&messageTotalSize, headerBuf, HEADER_SIZE);

  return messageTotalSize;
}

void network::readBody(int socketFD, inmessage &msg, unsigned int totalSize)
{
  int remainingMessageSize = totalSize - HEADER_SIZE;

  msg.allocate(remainingMessageSize);

  unsigned char* position = msg.data;

  while(true)
  {
    int rc = recv(socketFD, position, remainingMessageSize, 0);

    if ( rc == 0 )
    {
      close(socketFD);
      throw socket_closed_exception();
    }
    else if ( rc == -1 )
    {
      close(socketFD);
      throw socket_error_exception();
    }
    else if( rc != remainingMessageSize)
    {
       remainingMessageSize = remainingMessageSize - rc;
       position += rc;
    }
    else break;
  }
}

void network::recvMessage(int socketFD, inmessage &msg)
{
  unsigned int totalSize = readHeader(socketFD);
  readBody(socketFD, msg, totalSize);
}

void network::sendMessage(int socketFD, outmessage &msg)
{
  int totalSize = sizeof(int) + msg.size;

  unsigned char buffer[totalSize];
  memcpy(buffer, &totalSize, sizeof(totalSize));
  memcpy(buffer + sizeof(totalSize), msg.data, msg.size);
 
  int remainingMessageSize = totalSize; 
  unsigned char* position = buffer;

  while(true)
  {
    int rc = send(socketFD, position, remainingMessageSize, 0);

    if ( rc == 0 )
    {
      close(socketFD);
      throw socket_closed_exception();
    }
    else if ( rc == -1 )
    {
      close(socketFD);
      throw socket_error_exception();
    }
    else if( rc != remainingMessageSize)
    {
       remainingMessageSize = remainingMessageSize - rc;
       position += rc;
    }
    else break;
  }
}

}
