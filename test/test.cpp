#include "thread_pool.h"
#include "ibstream.h"
#include "obstream.h"
#include "serverstream.h"
#include "clientstream.h"
#include "safe_map.h"
#include "endianess.h"

struct processor
{
  void addClient(int newSocketFD, sockaddr_in clientAddr) { }

  void removeClient(int socketFD) { }

  void recvMessage(int socketFD) { }
};

void f1()
{
  alm::thread_pool pool(2);
  alm::ibstream<alm::same> input(5);

  processor p;
  alm::serverstream<processor> server(1100, p);

  alm::clientstream client;

  alm::safe_map<int, int> map;
}

int main()
{
  alm::obstream<alm::same> o;
  int a = 0;
  o << a << a;
}
