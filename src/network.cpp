#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "network.h"
#include "exceptions.h"

namespace alm
{
message::message()
  : size(0), data(0)
{
}

message::~message()
{
  if(data)
  {
    delete[] data;
  }
}

void message::allocate(unsigned int msgSize)
{
  if(data)
  {
    throw message_allocation_exception();
  }
  size = msgSize;
  data = new unsigned char[size];
}

const int network::HEADER_SIZE = sizeof(int);

unsigned int network::readHeader(int socketFD)
{
  unsigned int messageTotalSize = 0;

  unsigned char headerBuf[HEADER_SIZE];

  int rc = read(socketFD, headerBuf, HEADER_SIZE);

  if ( rc == 0 )
  {
    throw socket_closed_exception();
  }
  else if ( rc == -1 )
  {
    throw socket_error_exception();
  }
  else if ( rc != HEADER_SIZE )
  {
    throw read_header_exception();
  }
   
  memcpy(&messageTotalSize, headerBuf, HEADER_SIZE);

  return messageTotalSize;
}

void network::readBody(int socketFD, message &msg, unsigned int totalSize)
{
  int remainingMessageSize = totalSize - HEADER_SIZE;

  msg.allocate(remainingMessageSize);

  unsigned char* position = msg.data;

  while(true)
  {
    int rc = read(socketFD, position, remainingMessageSize);

    if ( rc == 0 )
    {
      throw socket_closed_exception();
    }
    else if ( rc == -1 )
    {
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

void network::recv(int socketFD, message &msg)
{
  unsigned int totalSize = readHeader(socketFD);
  readBody(socketFD, msg, totalSize);
}

void network::send(int socketFD, unsigned char* data, unsigned int size)
{
  int totalSize = sizeof(int) + size;

  unsigned char buffer[totalSize];
  memcpy(buffer, &totalSize, sizeof(totalSize));
  memcpy(buffer + sizeof(totalSize), data, size);
 
  int remainingMessageSize = totalSize; 
  unsigned char* position = buffer;

  while(true)
  {
    int rc = write(socketFD, position, remainingMessageSize);

    if ( rc == 0 )
    {
      throw socket_closed_exception();
    }
    else if ( rc == -1 )
    {
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
