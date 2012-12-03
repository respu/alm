#ifndef __ALM__CLIENT_STREAM__
#define __ALM__CLIENT_STREAM__

#include <arpa/inet.h>
#include <sys/poll.h>
#include <string>
#include <string.h>
#include <thread>
#include <atomic>
#include "exceptions.h"
#include "messages.h"
#include "network.h"

namespace alm
{

template<typename processor>
class clientstream
{
public:
  clientstream()
  : m_timeout(0), m_running(false), m_port(0), m_processor(0),
    m_socketFD(0)
  {
    memset(&m_sockAddr, 0, sizeof(m_sockAddr));
  }

  clientstream(clientstream const&) = delete;
  
  clientstream& operator =(clientstream const&) = delete;

  ~clientstream()
  {
    stop();
  }

  void start(const char* ip, unsigned short port,
             processor &proc, unsigned int timeout)
  {
    m_running = true;

    m_ip = ip;
    m_port = port;
    m_processor = &proc;
    m_timeout = timeout;

    init();

    m_thread = std::thread(&clientstream<processor>::run, this);
  }

  void stop()
  {
    m_running = false;

    if(m_thread.joinable())
    {
      m_thread.join();
    }
  }

  void sendMessage(outmessage &msg)
  {
    network::sendMessage(m_socketFD, msg);
  }

private:
  unsigned int m_timeout;

  std::atomic<bool> m_running;

  unsigned short m_port;

  processor* m_processor;

  int m_socketFD;

  sockaddr_in m_sockAddr;

  struct pollfd m_pollSocket;

  std::thread m_thread;

  std::string m_ip;

  void init()
  {
    createSocket();

    connectSocket();    
  }

  void createSocket()
  {
    m_socketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    m_pollSocket.fd = m_socketFD;
    m_pollSocket.events = POLLIN;

    if (-1 == m_socketFD)
    {
      throw create_socket_exception();
    }

    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_port = htons(m_port);
    int res = inet_pton(AF_INET, m_ip.c_str(), &m_sockAddr.sin_addr);

    if (0 > res)
    {
      close(m_socketFD);
      throw address_family_exception();
    }
    else if (0 == res)
    {
      close(m_socketFD);
      throw ip_address_exception(); 
    }
  }

  void connectSocket()
  {
    if (-1 == connect(m_socketFD, (sockaddr*)&m_sockAddr, sizeof(m_sockAddr)))
    {
      close(m_socketFD);
      throw connect_failed_exception();
    }
  }

  void run()
  {
    while(m_running)
    {
      process();
    }
    
    close(m_socketFD);
  }

  void process()
  {
    /* Block until input arrives on the sockets. */
    if(pollSocket())
    {
      /* Service the socket with input pending. */
      if(m_pollSocket.revents == POLLIN)
      {
        try
        {
          m_processor->recvMessage(m_pollSocket.fd);
        }
        catch(socket_closed_exception &e)
        {
          /* Stop running */
          m_running = false;
        }
      }
    }
  }

  bool pollSocket()
  {
    bool result = false;

    int rc = poll(&m_pollSocket, 1, m_timeout);
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
};

}

#endif
