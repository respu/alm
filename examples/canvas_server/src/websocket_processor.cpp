#include "alm/network.h"
#include "alm/websocket.h"
#include "websocket_processor.h"

websocket_processor::websocket_processor(const char* base)
  : m_http_handler(base)
{
}

void websocket_processor::onOpen(int newSocketFD, sockaddr_in clientAddr)
{
}

void websocket_processor::onClose(int socketFD)
{
  if(m_websocket_handler.exists(socketFD))
  {
    m_websocket_handler.onClose(socketFD);
  }
}

void websocket_processor::onMessage(int socketFD)
{
  int rc = alm::network::recv(socketFD, m_buffer, BLOCK);
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
	m_websocket_handler.onOpen(socketFD);
      }
      else
      {
	m_http_handler.process(socketFD, m_buffer, rc);
      }
    }
  }
}
