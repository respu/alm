#ifndef __ALM__TCP_CLIENT__
#define __ALM__TCP_CLIENT__

#include <arpa/inet.h>
#include <sys/poll.h>
#include <string>
#include <string.h>
#include <thread>
#include <atomic>
#include "alm/exceptions.h"
#include "alm/tcp.h"

namespace alm
{

template<typename handler>
class tcp_client
{
public:
  tcp_client()
  : m_timeout(0), m_running(false), m_port(0), m_handler(0),
    m_socketFD(0)
  {
    memset(&m_sockAddr, 0, sizeof(m_sockAddr));
  }

  tcp_client(tcp_client const&) = delete;
  
  tcp_client& operator =(tcp_client const&) = delete;

  ~tcp_client()
  {
    stop();
  }

  void start(const char* ip, unsigned short port,
             handler &_handler, unsigned int timeout)
  {
    m_running = true;

    m_ip = ip;
    m_port = port;
    m_handler = &_handler;
    m_timeout = timeout;

    init();

    m_thread = std::thread(&tcp_client<handler>::run, this);
  }

  void stop()
  {
    m_running = false;

    if(m_thread.joinable())
    {
      m_thread.join();
    }
  }

  void sendMessage(unsigned char* data, unsigned int size)
  {
    m_handler->sendMessage(m_socketFD, data, size);
  }

private:
  unsigned int m_timeout;

  std::atomic<bool> m_running;

  unsigned short m_port;

  handler* m_handler;

  int m_socketFD;

  sockaddr_in m_sockAddr;

  struct pollfd m_pollSocket;

  std::thread m_thread;

  std::string m_ip;

  void init()
  {
    m_socketFD = tcp::createSocket(m_port, m_sockAddr);
    m_pollSocket.fd = m_socketFD;
    m_pollSocket.events = POLLIN;

    tcp::connectSocket(m_socketFD, m_sockAddr);
  }

  void run()
  {
    while(m_running)
    {
      process();
    }
    tcp::closeSockets(&m_pollSocket, 1);
  }

  void process()
  {
    /* Block until input arrives on the sockets. */
    if(tcp::pollSocket(&m_pollSocket, 1, m_timeout))
    {
      /* Service the socket with input pending. */
      if(m_pollSocket.revents == POLLIN)
      {
        try
        {
          m_handler->onMessage(m_pollSocket.fd);
        }
        catch(socket_closed_exception &e)
        {
          /* Stop running */
          m_running = false;
        }
      }
    }
  }
};

}

#endif
