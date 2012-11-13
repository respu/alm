#ifndef __ALM__CLIENT_STREAM__
#define __ALM__CLIENT_STREAM__

#include <arpa/inet.h>
#include <string>
#include "exceptions.h"
#include "messages.h"

namespace alm
{

class clientstream
{
public:
  clientstream();

  ~clientstream();

  void openSocket(const char* ip, unsigned short port);

  void closeSocket();

  void sendMessage(outmessage &msg);

private:
  static const int HEADER_SIZE;

  std::string m_ip;

  unsigned short m_port;

  sockaddr_in m_sockAddr;

  int m_socketFD;

  void createSocket();

  void connectSocket();
};

}

#endif
