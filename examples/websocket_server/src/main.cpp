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
    std::cout << "URL: " << url << std::endl;

    std::string fileName = base + url;
    alm::http::responseFile(socketFD, fileName);
  }

  void doPost(int socketFD, const std::string &url, const std::string &message)
  {
    std::cout << "URL: " << url << std::endl;
    std::cout << "Message: " << message << std::endl;

    std::string fileName = base + url;
    alm::http::responseFile(socketFD, fileName);
  }

private:
  std::string base;
};

class ws_handler
{
public:
  ws_handler()
    : m_pool(2), m_http_handler("/home/alem/Workspace/web/")
  {
  }

  void addClient(int newSocketFD, alm::websocket_conn status)
  {
    m_clients.insert(newSocketFD, status); 
  }

  void removeClient(int socketFD)
  {
    m_clients.erase(socketFD, [] (int){}); 
  }

  alm::websocket_conn getClientStatus(int socketFD)
  {
    return m_clients.find(socketFD);
  }

  void processFrame(int socketFD, alm::websocket_frame &frame)
  {
    alm::websocket_frame m(std::move(frame));

    std::cout << "Frame: " << m.data.size() <<  std::endl;
    std::cout.write((const char*)m.data.data(), m.data.size());
    std::cout << std::endl;

    alm::websocket<ws_handler>::writeFrame(socketFD, m.data.data(), m.data.size(),
                                           frame.header.opcode);
  }

  void processHttp(int socketFD, unsigned char* data, unsigned int size)
  {
    alm::http::request(socketFD, data, size, m_http_handler); 
  }

private:
  alm::thread_pool                        m_pool;

  alm::safe_map<int, alm::websocket_conn> m_clients;

  http_handler                            m_http_handler;
};

int main(void)
{
  ws_handler handler;
  alm::websocket<ws_handler> protocol(handler);
  alm::tcp_server<alm::websocket<ws_handler>> websocket_server;
  websocket_server.start(1100, protocol, 5000);

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


