#ifndef __WEBSOCKET_PROCESSOR__
#define __WEBSOCKET_PROCESSOR__

#include <string>
#include <arpa/inet.h>
#include "http_handler.h"
#include "websocket_handler.h"

class websocket_processor
{
public:
  websocket_processor(const char* base);

  void onOpen(int newSocketFD, sockaddr_in clientAddr);

  void onClose(int socketFD);
  
  void onMessage(int socketFD);
  
private:
  static const unsigned short BLOCK = 16384;

  unsigned char     m_buffer[BLOCK];

  http_handler      m_http_handler;

  websocket_handler m_websocket_handler;
};

#endif
