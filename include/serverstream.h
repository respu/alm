#ifndef __ALM__SERVER_STREAM__
#define __ALM__SERVER_STREAM__

#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <atomic>
#include "network.h"
#include "exceptions.h"
#include "messages.h"

namespace alm
{

template<typename Processor>
class serverstream
{
public:
  serverstream(unsigned short port, Processor &processor)
    : m_running(false), m_port(port), m_processor(processor),
      m_socketFD(0), m_maxFD(0)
  {
    memset(&m_sockAddr, 0, sizeof(m_sockAddr));
  }

  serverstream(serverstream const&) = delete;
  
  serverstream& operator =(serverstream const&) = delete;

  ~serverstream()
  {
    stop(); 
  }

  void start()
  {
    m_running = true;

    init();

    m_thread = std::thread(&serverstream<Processor>::run, this);
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
  std::atomic<bool> m_running;

  unsigned short m_port;

  Processor& m_processor;

  sockaddr_in m_sockAddr;

  int m_socketFD;

  int m_maxFD;

  timeval m_timeout;

  fd_set m_active_fd_set;

  fd_set m_read_fd_set;

  std::thread m_thread;

  void init()
  {
    createSocket();

    bindSocket();

    listenSocket();

    initSockets();
  }

  void createSocket()
  {
    m_socketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    m_maxFD = m_socketFD;

    if(-1 == m_socketFD)
    {
      throw create_socket_exception(); 
    }

    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_port = htons(m_port);
    m_sockAddr.sin_addr.s_addr = INADDR_ANY; 
  }

  void bindSocket()
  {
    if(-1 == bind(m_socketFD,(sockaddr*)&m_sockAddr, sizeof(m_sockAddr)))
    {
      close(m_socketFD);
      throw bind_socket_exception();
    }
  }

  void listenSocket()
  {
    if(-1 == listen(m_socketFD, 10))
    {
      close(m_socketFD);
      throw listen_socket_exception();
    }
  }

  void initSockets()
  {
    FD_ZERO(&m_active_fd_set);
    FD_SET(m_socketFD, &m_active_fd_set);
  }

  void run()
  {
    while(m_running)
    {
      process();
    }

    closeSockets();
  }

  void process()
  {
    /* Block until input arrives on one or more active sockets. */
    selectSocket();

    /* Service all the sockets with input pending. */
    for (int fd = 0; fd < m_maxFD + 1; ++fd)
    {
      if (FD_ISSET (fd, &m_read_fd_set))
      {
        if (fd == m_socketFD)
        {
          /* Connection request on original socket. */
          addClient(); 
        }
        else
        {
          /* Data arriving on an already-connected socket. */
          try
          {
            m_processor.recvMessage(fd);
          }
          catch(socket_closed_exception &e)
          {
            /* Remove the closed socket from the set */
            removeClient(fd);
          }
        }
      }
    }
  }

  void selectSocket()
  {
    m_timeout = {5, 0};
    m_read_fd_set = m_active_fd_set;
    if (select (FD_SETSIZE, &m_read_fd_set, NULL, NULL, &m_timeout) < 0)
    {
      throw select_socket_exception();
    }
  }

  void closeSockets()
  {
    for (int fd = 0; fd < m_maxFD + 1; ++fd)
    {
      if (FD_ISSET (fd, &m_read_fd_set))
      {
        close(fd);
      }
    }
  }

  void addClient()
  {
    sockaddr_in clientAddr;
    unsigned int size = sizeof(clientAddr);
    int newSocket = accept(m_socketFD, (sockaddr*)&clientAddr, &size);
    if (newSocket < 0)
    {
      throw accept_socket_exception();
    }
    FD_SET (newSocket, &m_active_fd_set);
    m_maxFD = (m_maxFD < newSocket) ? newSocket : m_maxFD;

    m_processor.addClient(newSocket, clientAddr); 
  }

  void removeClient(int socketFD)
  {
    FD_CLR(socketFD, &m_active_fd_set);

    m_processor.removeClient(socketFD);
  }
};

}

#endif
