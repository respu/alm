#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include "thread_pool.h"
#include "serverstream.h"
#include "http_processor.h"
#include "exceptions.h"
#include "safe_map.h"
#include "sha1.h"
#include "base64.h"

class httpProcessor
{
public:
  httpProcessor(const char* basedir) :pool(2), base(basedir) { }

  ~httpProcessor() { }

  void doGet(int socketFD, const std::string &url)
  {
    std::cout << "URL: " << url << std::endl;

    std::string fileName = base + url;
    pool.submit([&, socketFD, fileName]
      {
        alm::http_processor<httpProcessor>::responseFile(socketFD, fileName);
      });
  }

  void doPost(int socketFD, const std::string &url, const std::string &message)
  {
    std::cout << "URL: " << url << std::endl;
    std::cout << "Message: " << message << std::endl;

    std::string fileName = base + url;
    pool.submit([&, socketFD, fileName]
      {
        alm::http_processor<httpProcessor>::responseFile(socketFD, fileName);
      });
  }

private:
  alm::thread_pool pool;

  std::string base;
};

enum websocket_conn { CONNECTING, OPEN };

struct websocket_incomplete_frame_exception : std::exception {};

struct frame_header
{
  unsigned char fin;           // 1st bit
  unsigned char opcode;        // low 4 bits 
    // 2nd byte
  unsigned char masked;        // 1st bit
  unsigned char length;        // low 7 bits

  // 3rd - 9th bytes
  unsigned int payload_length;
  unsigned int pos;

  unsigned int mask;
};

template<typename handler>
class websocket_processor
{
public:
  websocket_processor(handler& h)
    : MAGIC_KEY("258EAFA5-E914-47DA-95CA-C5AB0DC85B11"),
      m_handler(h)
  {
  }

  void addClient(int newSocketFD, sockaddr_in clientAddr)
  {
    std::cout << "ip: " << inet_ntoa(clientAddr.sin_addr) <<
		" port: " << ntohs(clientAddr.sin_port) <<
		" socketFD: " << newSocketFD << std::endl;
    websocket_conn status = CONNECTING;
    m_clients.insert(newSocketFD, status); 
  }

  void removeClient(int socketFD)
  {
    std::cout << "Closed client socket " << socketFD << std::endl;

    m_clients.erase(socketFD, [] (int){}); 
  }

  void recvMessage(int socketFD)
  {
    std::cout << "recvMessage: " << socketFD << std::endl;

    char input[8096];
    int rc =read(socketFD, input, 8096);
    if ( rc == 0 )
    {
      throw alm::socket_closed_exception();
    }
    else if ( rc == -1 )
    {
      throw alm::socket_error_exception();
    }
    else if( rc > 0)
    {
//      std::cout << "Message: " << std::endl;
//      std::cout.write(input, rc);
//      std::cout << std::endl; 

      request(socketFD, input, rc);
    }
  }

  void request(int socketFD, char* input, int length)
  {
    websocket_conn status = m_clients.find(socketFD);
    if(status == CONNECTING)
    {
      handshake(socketFD, input, length);
      websocket_conn status = OPEN;
      m_clients.insert(socketFD, status);
    }
    else if(status == OPEN)
    {
      readFrames(socketFD, input, length);
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

  void handshake(int socketFD, char* input, int length)
  {
    std::stringstream ss;
    ss.write(input, length);

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

  void readFrames(int socketFD, char* input, int in_length)
  {
    alm::inmessage msg;

    frame_header header;
    parseFrameHeader(input, in_length, header);

    unsigned int pos = header.pos;
    parseFramePayload(socketFD, input, in_length, header, pos, msg);

    while(msg.size < header.payload_length)
    {
      pos = 0;
      int buffer_size = header.payload_length - msg.size; 
      char buffer[buffer_size];
      int rc =read(socketFD, buffer, buffer_size);
      parseFramePayload(socketFD, buffer, rc, header, pos, msg);
    }

    m_handler.doFrame(socketFD, msg, header.opcode);
  }

  void parseFrameHeader(char* input, int in_length, frame_header &header)
  {
    if(in_length < 3)
    {
      throw websocket_incomplete_frame_exception();
    }

    // 1st byte
    header.fin    = input[0] & 0x80; // 1st bit
    header.opcode = input[0] & 0x0F; // low 4 bits 
    // 2nd byte
    header.masked = input[1] & 0x80; // 1st bit
    header.length = input[1] & 0x7F; // low 7 bits

    // 3rd - 9th bytes
    header.payload_length = header.length;
    header.pos = 2;
    if(header.length == 126)
    {
      unsigned short network_length = *((unsigned short*)(input + header.pos));
      header.payload_length = ntohs(network_length);  
      header.pos += 2;
    }
    else if(header.length == 127)
    {
      // TODO: 64-bit long
      unsigned long long network_length = *((unsigned short*)(input + header.pos));
      header.payload_length = ntohl(network_length);
      header.pos += 8;
    }

    header.mask = 0;
    if(header.masked)
    {
      header.mask = *((unsigned int*)(input + header.pos));
      header.pos += 4;
    }
  }

  void parseFramePayload(int socketFD, char* input, unsigned int in_length,
                  frame_header &header, unsigned int pos, alm::inmessage &msg)
  {
    if(header.masked)
    {
      // unmask data:
      char* c = input + pos;
      unsigned int max = header.payload_length > in_length ?
                         in_length : header.payload_length;
      for(unsigned int i=0; i<max; i++)
      {
	c[i] = c[i] ^ ((char*)(&header.mask))[i%4];
      }
    }
    unsigned int written_length = header.payload_length > in_length ?
                                  in_length - pos : header.payload_length;
    msg.write((unsigned char*)input + pos, written_length);
  }

  static unsigned int writeFrameHeader(unsigned char* header, unsigned char* data,
                                       unsigned int size, char opcode)
  {
    // 1st byte
    header[0]  = 0x80;   // 1st bit (FIN)
    header[0] |= opcode; // lower 4 bits (OPCODE)

    unsigned int pos = 1;
    if(size<=125)
    {
      header[pos++] = size;
    }
    else if(size<=65535)
    {
      header[pos++] = 126; //16 bit length

      unsigned short host_length = (unsigned short)(size);
      unsigned short network_length = htons(host_length);
      memcpy(header + pos, &network_length, sizeof(network_length));
      pos += sizeof(network_length);
    }
    else
    { // >2^16-1
      //TODO: write 8 bytes length

      header[pos++] = 127; //64 bit length

      unsigned int host_length = size;
      unsigned int network_length = htonl(host_length);
      memcpy(header + pos, &network_length, sizeof(network_length));

      pos+=8;
    }

    return pos;
  }

  static void writeFrame(int socketFD, unsigned char* data, unsigned int size, char opcode)
  {
    unsigned char header[14];
    unsigned int pos = writeFrameHeader(header, data, size, opcode);
     
    int response_length = pos + size;
    char response[response_length];

    memcpy(response, header, pos);
    memcpy(response + pos, data, size);

    write(socketFD, response, response_length);
  }

private:
  const std::string MAGIC_KEY;

  alm::safe_map<int, websocket_conn> m_clients;

  handler& m_handler;
};

struct websocket_handler
{
  void doFrame(int socketFD, alm::inmessage &msg, char opcode)
  {
    alm::inmessage m(std::move(msg));

    std::cout << "Frame: " << std::endl;
    std::cout.write((char*)m.data, m.size);
    std::cout << std::endl;

    std::cout << "Opcode: " << opcode << std::endl;

    alm::outmessage out;
    out.data = m.data;
    out.size = m.size;

    websocket_processor<websocket_handler>::writeFrame(socketFD, out.data, out.size, opcode);
  }
};

int main(void)
{
  httpProcessor p("/home/alem/Workspace/web/");
  alm::http_processor<httpProcessor> http_p(p);
  alm::serverstream<alm::http_processor<httpProcessor>> http_server;
  http_server.start(1100, http_p, 5000);

  websocket_handler handler;
  websocket_processor<websocket_handler> websocket_p(handler);
  alm::serverstream<websocket_processor<websocket_handler>> websocket_server;
  websocket_server.start(1101, websocket_p, 5000);

  std::string line;
  while (std::getline(std::cin, line))
  {
    if(line.compare("quit") == 0)
    {
      break;
    }
  }

  websocket_server.stop();
  http_server.stop();
}


