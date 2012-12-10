#ifndef __ALM__NETWORK__
#define __ALM__NETWORK__

namespace alm
{

struct message
{
  unsigned int size;
  unsigned char* data;

  message();

  ~message();

  void allocate(unsigned int msgSize);
};

class network
{
public:
  static void recv(int socketFD, message &msg);

  static void send(int socketFD, unsigned char* data, unsigned int size); 

private:
  static const int HEADER_SIZE; 

  static unsigned int readHeader(int socketFD);

  static void readBody(int socketFD, message &msg, unsigned int totalSize);
};

}

#endif
