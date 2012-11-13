#include "thread_pool.h"
#include "ibstream.h"
#include "obstream.h"
#include "serverstream.h"
#include "clientstream.h"

struct processor
{
  void addClient(int newSocketFD, sockaddr_in clientAddr) { }

  void removeClient(int socketFD) { }

  void recvMessage(int socketFD) { }
};

int main()
{
  alm::thread_pool pool(2);
  alm::ibstream input(5);
  alm::obstream output;

  processor p;
  alm::serverstream<processor> server(1100, p);

  alm::clientstream client;
}
