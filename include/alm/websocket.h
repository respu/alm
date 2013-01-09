#ifndef __ALM__WEBSOCKET__
#define __ALM__WEBSOCKET__

#include <string>
#include "alm/ibstream.h"

namespace alm
{


struct websocket_frame_header
{
  unsigned short header_length;

  // 1st byte
  unsigned char fin;           // 1st bit
  unsigned char opcode;        // low 4 bits 
  // 2nd byte
  unsigned char masked;        // 1st bit
  unsigned char data_length;        // low 7 bits
  // 3rd - 9th bytes
  unsigned long long data_length_ext;
  unsigned int mask;

  websocket_frame_header();
  websocket_frame_header(const websocket_frame_header &other);
};

struct websocket_frame
{
  websocket_frame_header header;
  ibstream          data;

  websocket_frame();
  websocket_frame(websocket_frame &&other);

  websocket_frame(const websocket_frame &other)             = delete;
  websocket_frame& operator= (const websocket_frame &other) = delete;
};

class websocket
{
public:
  static const std::string SEC_WEBSOCKET_KEY;

  static void handshake(int socketFD, std::string &rqst);
  
  static void readFrame(int socketFD, unsigned char* data, unsigned int size,
                        websocket_frame &frame);
  
  static void writeFrame(int socketFD, unsigned char* data,
                       unsigned long long size, char opcode);
  
private:
  static const std::string MAGIC_KEY;

  static std::string getKey(std::string &rqst);

  static std::string bigEndian(std::string &hashed);

  static std::string hashKey(std::string &key);
  
  static void parseFrameHeader(unsigned char* data, unsigned int size,
                        websocket_frame_header &header);

  static void unmaskFrame(websocket_frame &frame);
  
  static unsigned long long parseFramePayload(unsigned char* data,
                                       unsigned int size, websocket_frame &frame,
                                       unsigned long long maskIndex);
  
  static unsigned int writeFrameHeader(unsigned char* header, unsigned char* data,
                                       unsigned long long size, char opcode);
};

}
#endif
