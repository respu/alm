#include <iostream>
#include <string>
#include "tcp.h"
#include "tcp_server.h"
#include "websocket.h"
#include "network.h"
#include "endianess.h"
#include "http_handler.h"
#include "websocket_handler.h"

class ws_processor
{
public:
  ws_processor()
    : m_http_handler("/home/alem/Workspace/alm/examples/canvas_server/web")
  {
  }

  void addClient(int newSocketFD, sockaddr_in clientAddr) { }

  void removeClient(int socketFD)
  {
    m_websocket_handler.removeClient(socketFD); 
  }

  void recvMessage(int socketFD)
  {
    int rc = alm::network::readData(socketFD, m_buffer, BLOCK);
    if( rc > 0)
    {
      if(m_websocket_handler.exists(socketFD))
      {
        alm::websocket_frame frame;
        alm::websocket::readFrame(socketFD, m_buffer, rc, frame);
        m_websocket_handler.processFrame(socketFD, frame);
      }
      else
      {
        std::string rqst((const char*)m_buffer, rc);
        if(rqst.find(alm::websocket::SEC_WEBSOCKET_KEY) != std::string::npos)
        {
          alm::websocket::handshake(socketFD, rqst);
          m_websocket_handler.addClient(socketFD);
        }
        else
        {
          m_http_handler.process(socketFD, m_buffer, rc);
        }
      }
    }
  }

private:
  static const unsigned short BLOCK = 16384;

  unsigned char     m_buffer[BLOCK];

  http_handler      m_http_handler;

  websocket_handler m_websocket_handler;
};

int main(void)
{
  ws_processor processor;
  alm::tcp_server<ws_processor> websocket_server;
  websocket_server.start(1100, processor, 5000);

  std::string line;
  while (std::getline(std::cin, line))
  {
    if(line.compare("quit") == 0)
    {
      break;
    }
  }

  websocket_server.stop();
}


