#include <iostream>
#include <string>
#include <thread>
#include "tcp.h"
#include "tcp_server.h"
#include "http.h"
#include "exceptions.h"
#include "websocket.h"
#include "network.h"
#include "safe_map.h"
#include "endianess.h"

class http_handler
{
public:
  http_handler(const char* basedir) : base(basedir) { }

  ~http_handler() { }

  void process(int socketFD, unsigned char* data, unsigned int size)
  {
    alm::http_request request;
    try
    {
      alm::http::parseRequest(socketFD, data, size, request);
    }
    catch(alm::forbidden_exception &e)
    {
      alm::http::forbidden(socketFD);
    }

    std::string fileName = base + request.url;
    try
    {
      alm::http::responseFile(socketFD, fileName);
    }
    catch(alm::file_not_found_exception &e)
    {
      alm::http::notFound(socketFD);
    }
  }

private:
  std::string base;
};

class websocket_handler
{
public:
  websocket_handler()
  {
    m_thread = std::thread([&]
      {
        while(true)
        {
          m_clients.for_each([]
            (int socketFD)
            {
              float bn = 1.5;
              //unsigned int bn = alm::big::uint(ln);
              alm::websocket::response(socketFD, (unsigned char*)&bn, sizeof(bn), 2);
            });
          sleep(1);
        }
      });
  }

  ~websocket_handler()
  {
    if(m_thread.joinable())
    {
      m_thread.join();
    } 
  }

  void addClient(int newSocketFD)
  {
    std::cout << "addClient: " << newSocketFD << std::endl;

    m_clients.insert(newSocketFD, newSocketFD);
  }

  void removeClient(int socketFD)
  {
    std::cout << "removeClient: " << socketFD << std::endl;

    try
    {
      m_clients.erase(socketFD, [](int){});
    }
    catch(alm::not_found_exception &e)
    {
      std::cout << "[not found] removeClient: " << socketFD << std::endl;
    }
  }

  bool exists(int socketFD)
  {
    bool result = true;
    try
    {
      m_clients.find(socketFD);
    }
    catch(alm::not_found_exception &e)
    {
      result = false;
    }
    return result;
  }

  void processFrame(int socketFD, alm::websocket_frame &frame)
  {
    alm::websocket_frame m(std::move(frame));

    alm::websocket::response(socketFD, m.data.data(), m.data.size(),
                             frame.header.opcode);
  }

private:
  alm::safe_map<int, int> m_clients;

  std::thread m_thread;
};


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
        alm::websocket::parseFrame(socketFD, m_buffer, rc, frame);
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


