#include <iostream>
#include "websocket_handler.h"
#include "parameter.h"

websocket_handler::websocket_handler()
  : m_running(true)
{
  m_thread = std::thread([&]
    {
      while(m_running)
      {
	m_clients.for_each([]
	  (int socketFD)
	  {
            parameter param;
            parameter_processor::next(param);
	    alm::websocket::writeFrame(socketFD, (unsigned char*)&param, sizeof(param), 2);
	  });
	sleep(1);
      }
    });
}

websocket_handler::~websocket_handler()
{
  m_running = false;

  if(m_thread.joinable())
  {
    m_thread.join();
  } 
}

void websocket_handler::addClient(int newSocketFD)
{
  m_clients.insert(newSocketFD, newSocketFD);
}

void websocket_handler::removeClient(int socketFD)
{
  try
  {
    m_clients.erase(socketFD, [](int){});
  }
  catch(alm::not_found_exception &e)
  {
    std::cout << "[not found] removeClient: " << socketFD << std::endl;
  }
}

bool websocket_handler::exists(int socketFD)
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

void websocket_handler::processFrame(int socketFD, alm::websocket_frame &frame)
{
  alm::websocket_frame m(std::move(frame));

  alm::websocket::writeFrame(socketFD, m.data.data(), m.data.size(),
			   frame.header.opcode);
}
