#ifndef __ALM__TCP_SERVER__
#define __ALM__TCP_SERVER__

#include <arpa/inet.h>
#include <sys/poll.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <atomic>
#include "alm/exceptions.h"
#include "alm/tcp.h"

namespace alm
{

template<typename handler>
class tcp_server
{
public:
  tcp_server()
    : m_timeout(0),m_running(false), m_port(0), m_handler(0),
      m_listenFD(0), m_numSockets(0)
  {
    memset(&m_sockAddr, 0, sizeof(m_sockAddr));
  }

  tcp_server(tcp_server const&) = delete;
  
  tcp_server& operator =(tcp_server const&) = delete;

  ~tcp_server()
  {
    stop(); 
  }

  void start(unsigned short port, handler &_handler, unsigned int timeout)
  {
    m_running = true;

    m_port = port;
    m_handler = &_handler;
    m_timeout = timeout;

    init();

    m_thread = std::thread(&tcp_server<handler>::run, this);
  }

  void stop()
  {
    m_running = false;

    if(m_thread.joinable())
    {
      m_thread.join();
    }
  }

private:
  unsigned int m_timeout;

  std::atomic<bool> m_running;

  unsigned short m_port;

  handler* m_handler;

  sockaddr_in m_sockAddr;

  int m_listenFD;

  struct pollfd m_sockets[tcp::MAX_SOCKETS];

  int m_numSockets;

  std::thread m_thread;

  void init()
  {
    m_listenFD = tcp::createSocket(m_port, m_sockAddr);
    addSocket(m_listenFD);

    tcp::bindSocket(m_listenFD, m_sockAddr);

    tcp::listenSocket(m_listenFD);
  }

  void addSocket(int newSocketFD)
  {
    m_sockets[m_numSockets].fd = newSocketFD;
    m_sockets[m_numSockets].events = POLLIN;
    m_numSockets++;
  }

  void run()
  {
    while(m_running)
    {
      process();
    }

    tcp::closeSockets(m_sockets, m_numSockets);
  }

  void process()
  {
    /* Block until input arrives on one or more active sockets. */
    if(tcp::pollSocket(m_sockets, m_numSockets, m_timeout))
    {
      /* Service all the sockets with input pending. */
      int currentSockets = m_numSockets;
      for (int i = 0; i < currentSockets; ++i)
      {
        if(m_sockets[i].revents == POLLIN)
        {
          if (m_sockets[i].fd == m_listenFD)
          {
            /* Connection request on original socket. */
            newClient(); 
          }
          else
          {
            /* Data arriving on an already-connected socket. */
            newMessage(i); 
          }
        }
      }
    }
  }

  void newClient()
  {
    sockaddr_in clientAddr;
    unsigned int size = sizeof(clientAddr);
    int newSocketFD = accept(m_listenFD, (sockaddr*)&clientAddr, &size);
    if (newSocketFD < 0)
    {
      throw accept_socket_exception();
    }

    addSocket(newSocketFD);

    m_handler->onOpen(newSocketFD, clientAddr); 
  }

  void newMessage(int indexFD)
  {
    try
    {
      m_handler->onMessage(m_sockets[indexFD].fd);
    }
    catch(socket_closed_exception &e)
    {
      /* Remove the closed socket from the set */
      onClose(indexFD);
    }
  }

  void onClose(int indexFD)
  {
    m_handler->onClose(m_sockets[indexFD].fd);

    close(m_sockets[indexFD].fd);

    m_sockets[indexFD].fd = -1;
    for (int i = 0; i < m_numSockets; i++)
    {
      if (m_sockets[i].fd == -1)
      {
        for(int j = i; j < m_numSockets; j++)
        {
          m_sockets[j].fd = m_sockets[j+1].fd;
        }
        m_numSockets--;
      }
    }
  }
};

}

#endif
