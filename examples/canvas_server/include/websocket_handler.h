#ifndef __WEBSOCKET_HANDLER__
#define __WEBSOCKET_HANDLER__

#include <atomic>
#include <thread>
#include "alm/websocket.h"
#include "alm/safe_map.h"

class websocket_handler
{
public:
  websocket_handler();
  
  ~websocket_handler();
  
  void onOpen(int newSocketFD);

  void onClose(int socketFD);

  bool exists(int socketFD);

  void processFrame(int socketFD, alm::websocket_frame &frame);

private:
  alm::safe_map<int, int> m_clients;

  std::atomic<bool> m_running;

  std::thread m_thread;
};

#endif
