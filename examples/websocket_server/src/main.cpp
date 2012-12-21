#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdint.h>
#include "thread_pool.h"
#include "tcp_server.h"
#include "http.h"
#include "exceptions.h"
#include "safe_map.h"
#include "sha1.h"
#include "base64.h"
#include "ibstream.h"
#include "websocket.h"

class http_handler
{
public:
  http_handler(const char* basedir) : base(basedir) { }

  ~http_handler() { }

  void doGet(int socketFD, const std::string &url)
  {
    std::string fileName = base + url;
    alm::http::responseFile(socketFD, fileName);
  }

  void doPost(int socketFD, const std::string &url, const std::string &message)
  {
    std::string fileName = base + url;
    alm::http::responseFile(socketFD, fileName);
  }

private:
  std::string base;
};

class websocket_handler
{
public:
  websocket_handler()
  {
    memset(m_clients, -1, sizeof(m_clients));
  }

  void addClient(int newSocketFD)
  {
    m_clients[newSocketFD] = newSocketFD;
  }

  void removeClient(int socketFD)
  {
    m_clients[socketFD] = -1;
  }

  bool exists(int socketFD)
  {
    return m_clients[socketFD] >= 0;
  }

  void processFrame(int socketFD, alm::websocket_frame &frame)
  {
    alm::websocket_frame m(std::move(frame));

    alm::websocket::response(socketFD, m.data.data(), m.data.size(),
                             frame.header.opcode);
  }

private:
  short m_clients[200];

};


class ws_processor
{
public:
  ws_processor()
    : m_pool(2), m_http_handler("/home/alem/Workspace/web/")
  {
  }

  void addClient(int newSocketFD, sockaddr_in clientAddr)
  {
  }

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
          alm::http::request(socketFD, m_buffer, rc, m_http_handler); 
        }
      }
    }
  }

private:
  static const unsigned short BLOCK = 16384;

  unsigned char     m_buffer[BLOCK];

  alm::thread_pool  m_pool;

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


