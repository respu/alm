#ifndef __ALM__SERVER_STREAM__
#define __ALM__SERVER_STREAM__

#include <arpa/inet.h>
#include <sys/poll.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <atomic>
#include "network.h"
#include "exceptions.h"

namespace alm
{

template<typename processor>
class serverstream
{
public:
  serverstream()
    : m_timeout(0),m_running(false), m_port(0), m_processor(0),
      m_listenFD(0), m_numSockets(0)
  {
    memset(&m_sockAddr, 0, sizeof(m_sockAddr));
  }

  serverstream(serverstream const&) = delete;
  
  serverstream& operator =(serverstream const&) = delete;

  ~serverstream()
  {
    stop(); 
  }

  void start(unsigned short port, processor &proc, unsigned int timeout)
  {
    m_running = true;

    m_port = port;
    m_processor = &proc;
    m_timeout = timeout;

    init();

    m_thread = std::thread(&serverstream<processor>::run, this);
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
  static const int MAX_SOCKETS = 200;

  unsigned int m_timeout;

  std::atomic<bool> m_running;

  unsigned short m_port;

  processor* m_processor;

  sockaddr_in m_sockAddr;

  int m_listenFD;

  struct pollfd m_sockets[MAX_SOCKETS];

  int m_numSockets;

  std::thread m_thread;

  void init()
  {
    createSocket();

    bindSocket();

    listenSocket();
  }

  void addSocket(int newSocketFD)
  {
    m_sockets[m_numSockets].fd = newSocketFD;
    m_sockets[m_numSockets].events = POLLIN;
    m_numSockets++;
  }

  void createSocket()
  {
    m_listenFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    addSocket(m_listenFD);

    if(-1 == m_listenFD)
    {
      throw create_socket_exception(); 
    }

    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_port = htons(m_port);
    m_sockAddr.sin_addr.s_addr = INADDR_ANY; 
  }

  void bindSocket()
  {
    if(-1 == bind(m_listenFD,(sockaddr*)&m_sockAddr, sizeof(m_sockAddr)))
    {
      close(m_listenFD);
      throw bind_socket_exception();
    }
  }

  void listenSocket()
  {
    if(-1 == listen(m_listenFD, 10))
    {
      close(m_listenFD);
      throw listen_socket_exception();
    }
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
    if(pollSocket())
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

  bool pollSocket()
  {
    bool result = false;

    int rc = poll(m_sockets, m_numSockets, m_timeout);
    if (rc < 0)
    {
      throw poll_socket_exception();
    }
    else if(rc > 0)
    {
      result = true;
    }

    return result;
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

    m_processor->addClient(newSocketFD, clientAddr); 
  }

  void newMessage(int indexFD)
  {
    try
    {
      m_processor->recvMessage(m_sockets[indexFD].fd);
    }
    catch(socket_closed_exception &e)
    {
      /* Remove the closed socket from the set */
      removeClient(indexFD);
    }
  }

  void removeClient(int indexFD)
  {
    m_processor->removeClient(m_sockets[indexFD].fd);

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

  void closeSockets()
  {
    for(int i = 0; i < m_numSockets; i++)
    {
      if(m_sockets[i].fd >= 0)
      {
        close(m_sockets[i].fd);
      }
    }
  }
};

}

#endif
