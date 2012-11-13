#include <string.h>
#include "clientstream.h"
#include "network.h"

namespace alm
{

clientstream::clientstream()
  : m_port(0)
{
  memset(&m_sockAddr, 0, sizeof(m_sockAddr));
}

clientstream::~clientstream()
{
  closeSocket();
}

void clientstream::openSocket(const char* ip, unsigned short port)
{
  m_ip = ip;

  m_port = port;

  createSocket();

  connectSocket();
}

void clientstream::closeSocket()
{
  shutdown(m_socketFD, SHUT_RDWR);

  close(m_socketFD);
}

void clientstream::createSocket()
{
  m_socketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (-1 == m_socketFD)
  {
    throw create_socket_exception();
  }

  m_sockAddr.sin_family = AF_INET;
  m_sockAddr.sin_port = htons(m_port);
  int res = inet_pton(AF_INET, m_ip.c_str(), &m_sockAddr.sin_addr);

  if (0 > res)
  {
    close(m_socketFD);
    throw address_family_exception();
  }
  else if (0 == res)
  {
    close(m_socketFD);
    throw ip_address_exception(); 
  }
}

void clientstream::connectSocket()
{
  if (-1 == connect(m_socketFD, (sockaddr*)&m_sockAddr, sizeof(m_sockAddr)))
  {
    close(m_socketFD);
    throw connect_failed_exception();
  }
}

void clientstream::sendMessage(outmessage &msg)
{
  network::sendMessage(m_socketFD, msg);
}

}
