#ifndef __ALM__WEBSOCKET__
#define __ALM__WEBSOCKET__

#include <string>
#include <string.h>
#include <stdint.h>
#include "exceptions.h"
#include "safe_map.h"
#include "sha1.h"
#include "base64.h"
#include "ibstream.h"
#include "network.h"

namespace alm
{

enum websocket_conn { CONNECTING, OPEN };

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

  websocket_frame_header() { }

  websocket_frame_header(const websocket_frame_header &other)
    : header_length(other.header_length),fin(other.fin), opcode(other.opcode),
      masked(other.masked), data_length(other.data_length),
      data_length_ext(other.data_length_ext), mask(other.mask)
  {
  }
};

struct websocket_frame
{
  websocket_frame_header header;
  alm::ibstream          data;

  websocket_frame() { }

  websocket_frame(websocket_frame &&other)
    : header(other.header), data(std::move(other.data))
  {
  }

  websocket_frame(const websocket_frame &other)             = delete;
  websocket_frame& operator= (const websocket_frame &other) = delete;
};

template<typename handler>
class websocket
{
public:
  websocket(handler& h)
    : m_handler(h)
  {
  }

  void addClient(int newSocketFD, sockaddr_in clientAddr)
  {
    m_handler.addClient(newSocketFD, CONNECTING); 
  }

  void removeClient(int socketFD)
  {
    m_handler.removeClient(socketFD);
  }

  void recvMessage(int socketFD)
  {
    unsigned char input[BLOCK];
    int rc = alm::network::readData(socketFD, input, BLOCK);
    if( rc > 0)
    {
      request(socketFD, input, rc);
    }
  }

  void request(int socketFD, unsigned char* data, unsigned int size)
  {
    websocket_conn status = m_handler.getClientStatus(socketFD);
    if(status == CONNECTING)
    {
      handshake(socketFD, data, size);
      m_handler.addClient(socketFD, OPEN);
    }
    else if(status == OPEN)
    {
      readFrames(socketFD, data, size);
    }
  }

  std::string getKey(std::stringstream &ss)
  {
    std::string request = ss.str();

    int line = request.find("Sec-WebSocket-Key");
    int start = request.find(":", line) + 2;
    int end = request.find("\r", start);
    return request.substr(start, end-start);
  }

  std::string hashKey(std::string &key)
  {
    std::string combined = key + MAGIC_KEY;

    std::string hashed = alm::sha1::digest(combined);

    return alm::base64::encode(hashed);
  }

  void handshake(int socketFD, unsigned char* data, unsigned int size)
  {
    std::stringstream ss;
    ss.write((const char*)data, size);

    std::string key = getKey(ss);
    std::string hashedkey =  hashKey(key);

    std::stringstream response;
    response << "HTTP/1.1 101 Switching Protocols\r\n"
       << "Upgrade: WebSocket\r\n"
       << "Connection: Upgrade\r\n"
       << "Sec-WebSocket-Accept: " 
       << hashedkey << "\r\n"
       << "\r\n"; // Extra newline required by Websocket handshake
    std::string ack = response.str();

    write(socketFD, ack.c_str(), ack.length());
  }

  void readFrames(int socketFD, unsigned char* data, unsigned int size)
  {
    websocket_frame frame;

    parseFrameHeader(data, size, frame.header);

    unsigned long long maskIndex =
         parseFramePayload(data + frame.header.header_length, size - frame.header.header_length,
                           frame, 0);

    while(frame.data.size() < frame.header.data_length_ext)
    {
      unsigned char buffer[BLOCK];
      int rc =read(socketFD, buffer, BLOCK);
      maskIndex = parseFramePayload(buffer, rc, frame, maskIndex);
    }

    m_handler.doFrame(socketFD, frame);
  }

  void parseFrameHeader(unsigned char* data, unsigned int size,
                        websocket_frame_header &header)
  {
    if(size < 3)
    {
      throw alm::websocket_incomplete_frame_exception();
    }

    // 1st byte
    header.fin    = data[0] & 0x80; // 1st bit
    header.opcode = data[0] & 0x0F; // low 4 bits 
    // 2nd byte
    header.masked = data[1] & 0x80; // 1st bit
    header.data_length = data[1] & 0x7F; // low 7 bits

    // 3rd - 9th bytes
    header.data_length_ext = header.data_length;
    header.header_length = 2;
    if(header.data_length == 126)
    {
      unsigned short network_length = *((unsigned short*)(data + header.header_length));
      header.data_length_ext = ntohs(network_length);  
      header.header_length += sizeof(network_length);
    }
    else if(header.data_length == 127)
    {
      unsigned long long network_length = *((unsigned long long*)(data + header.header_length));
      header.data_length_ext = be64toh(network_length);
      header.header_length += sizeof(network_length);
    }

    header.mask = 0;
    if(header.masked)
    {
      header.mask = *((unsigned int*)(data + header.header_length));
      header.header_length += sizeof(header.mask);
    }
  }

  unsigned long long parseFramePayload(unsigned char* data,
                                       unsigned int size, websocket_frame &frame,
                                       unsigned long long maskIndex)
  {
    unsigned long long maxIndex = maskIndex + size;

    if(frame.header.masked)
    {
      // unmask data:
      unsigned char* c = data;
      for(unsigned long long i=0, j=maskIndex; i<size && j<maxIndex; ++i, ++j)
      {
	c[i] = c[i] ^ ((unsigned char*)(&frame.header.mask))[j%4];
      }
    }
    frame.data.write(data, size);

    return maxIndex;
  }

  static unsigned int writeFrameHeader(unsigned char* header, unsigned char* data,
                                       unsigned long long size, char opcode)
  {
    // 1st byte
    header[0]  = 0x80;   // 1st bit (FIN)
    header[0] |= opcode; // lower 4 bits (OPCODE)

    unsigned int header_length = 1;
    if(size<=125)
    {
      header[header_length++] = size;
    }
    else if(size<=65535)
    {
      header[header_length++] = 126; //16 bit length

      unsigned short host_length = size;
      unsigned short network_length = htons(host_length);
      memcpy(header + header_length, &network_length, sizeof(network_length));
      header_length += sizeof(network_length);
    }
    else
    {
      header[header_length++] = 127; //64 bit length

      unsigned long long host_length = size;
      unsigned long long network_length = htobe64(host_length);
      memcpy(header + header_length, &network_length, sizeof(network_length));
      header_length += sizeof(network_length);
    }

    return header_length;
  }

  static void writeFrame(int socketFD, unsigned char* data,
                         unsigned long long size, char opcode)
  {
    unsigned char header[10];
    unsigned int header_length = writeFrameHeader(header, data, size, opcode);

    alm::network::writeData(socketFD, header, header_length);
    alm::network::writeAllData(socketFD, data, size);
  }

private:
  static const short BLOCK = 16384;

  static const std::string MAGIC_KEY;

  handler& m_handler;
};

template<typename handler>
const std::string websocket<handler>::MAGIC_KEY = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

}
#endif
