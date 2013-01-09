#include <iostream>
#include <string>
#include "alm/tcp_server.h"
#include "websocket_processor.h"

int main(void)
{
  websocket_processor processor("/home/alem/Workspace/alm/examples/canvas_server/web");
  alm::tcp_server<websocket_processor> websocket_server;
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


