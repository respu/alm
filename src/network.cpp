#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "alm/network.h"
#include "alm/exceptions.h"

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

const int protocol::HEADER_SIZE = sizeof(int);

unsigned int protocol::readHeader(int socketFD)
{
  unsigned int messageTotalSize = 0;

  unsigned char headerBuf[HEADER_SIZE];

  if (network::readData(socketFD, headerBuf, HEADER_SIZE) != HEADER_SIZE)
  {
    throw read_header_exception();
  }
   
  memcpy(&messageTotalSize, headerBuf, HEADER_SIZE);

  return messageTotalSize;
}

void protocol::readBody(int socketFD, message &msg, unsigned int totalSize)
{
  int remaining = totalSize - HEADER_SIZE;

  msg.allocate(remaining);

  unsigned char* position = msg.data;

  int rc;
  while((rc = network::readData(socketFD, position, remaining)) != remaining)
  {
     remaining -= rc;
     position += rc;
  }
}

void protocol::recv(int socketFD, message &msg)
{
  unsigned int totalSize = readHeader(socketFD);
  readBody(socketFD, msg, totalSize);
}

void protocol::send(int socketFD, unsigned char* data, unsigned int size)
{
  int totalSize = HEADER_SIZE + size;

  network::writeData(socketFD, (unsigned char*)&totalSize, HEADER_SIZE); 
  network::writeAllData(socketFD, data, size); 
}

int network::readData(int socketFD, unsigned char* data, int size)
{
  int rc = read(socketFD, data, size);

  if ( rc == 0 )
  {
    throw socket_closed_exception();
  }
  else if ( rc == -1 )
  {
    throw socket_error_exception();
  }
  return rc;
}

int network::writeData(int socketFD, unsigned char* data, int size)
{
  int rc = write(socketFD, data, size);

  if ( rc == 0 )
  {
    throw socket_closed_exception();
  }
  else if ( rc == -1 )
  {
    throw socket_error_exception();
  }
  return rc;
}

void network::writeAllData(int socketFD, unsigned char* data, int size)
{
  int remaining = size; 
  unsigned char* position = data;

  int rc;
  while((rc = network::writeData(socketFD, position, remaining)) != remaining)
  {
    remaining -= rc;
    position += rc;
  }
} 

}
