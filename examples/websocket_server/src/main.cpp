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

enum webSocketConn { CONNECTING, OPEN };

struct websocket_incomplete_frame_exception : std::exception {};

class websocketProcessor
{
public:
  websocketProcessor()
    : MAGIC_KEY("258EAFA5-E914-47DA-95CA-C5AB0DC85B11")
  {
  }

  void addClient(int newSocketFD, sockaddr_in clientAddr)
  {
    std::cout << "ip: " << inet_ntoa(clientAddr.sin_addr) <<
		" port: " << ntohs(clientAddr.sin_port) <<
		" socketFD: " << newSocketFD << std::endl;
    webSocketConn status = CONNECTING;
    clients.insert(newSocketFD, status); 
  }

  void removeClient(int socketFD)
  {
    std::cout << "Closed client socket " << socketFD << std::endl;

    clients.erase(socketFD, [] (int){}); 
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
      request(socketFD, input, rc);
    }
  }

  void request(int socketFD, char* input, int length)
  {
    webSocketConn status = clients.find(socketFD);
    if(status == CONNECTING)
    {
      handshake(socketFD, input, length);
    }
    else if(status == OPEN)
    {
      readFrame(socketFD, input, length);
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

    webSocketConn status = OPEN;
    clients.insert(socketFD, status);
  }

  void readFrame(int socketFD, char* input, int in_length)
  {
    if(in_length < 3)
    {
      throw websocket_incomplete_frame_exception();
    }

    // 1st byte
    char fin    = input[0] & 0x80; // 1st bit
    char opcode = input[0] & 0x0F; // low 4 bits 
    // 2nd byte
    char masked = input[1] & 0x80; // 1st bit
    char length = input[1] & 0x7F; // low 7 bits

    // 3rd - 9th bytes
    int payload_length = length;
    int pos = 2;
    if(length == 126)
    {
      payload_length = input[2] + (input[3] << 8);
      pos += 2;
    }
    else if(length == 127)
    {
      payload_length = input[2] + (input[3] << 8);
      pos += 8;
    }

    if(in_length < pos + payload_length)
    {
      throw websocket_incomplete_frame_exception();
    }

    unsigned int mask = 0;
    if(masked)
    {
      mask = *((unsigned int*)(input + pos));
      pos += 4;

      // unmask data:
      char* c = input + pos;
      for(int i=0; i<payload_length; i++)
      {
	c[i] = c[i] ^ ((char*)(&mask))[i%4];
      }
    }

    char out_buffer[payload_length];
    memcpy(out_buffer, input + pos, payload_length);

    std::cout << "Frame: " << std::endl;
    std::cout.write(out_buffer, payload_length);
    std::cout << std::endl;

    std::cout << "Opcode: " << opcode << std::endl;

    writeFrame(socketFD, out_buffer, payload_length, opcode);
  }

  void writeFrame(int socketFD, char* output, int length, char opcode)
  {
    char header[10];

    // 1st byte
    header[0]  = 0x80;   // 1st bit (FIN)
    header[0] |= opcode; // lower 4 bits (OPCODE)

    int pos = 1;
    if(length<=125)
    {
      header[pos++] = length;
    }
    else if(length<=65535)
    {
      header[pos++] = 126; //16 bit length
      header[pos++] = (length >> 8) & 0xFF; // rightmost first
      header[pos++] = length & 0xFF;
    }
    else
    { // >2^16-1
      header[pos++] = 127; //64 bit length

      //TODO: write 8 bytes length
      pos+=8;
     }
     
     int response_length = length + pos;
     char response[response_length];

     memcpy(response, header, pos);
     memcpy(response + pos, output, length);

     write(socketFD, response, response_length);
  }

private:
  alm::safe_map<int, webSocketConn> clients;

  const std::string MAGIC_KEY; 
};

int main(void)
{
  httpProcessor p("/home/alem/Workspace/web/");
  alm::http_processor<httpProcessor> http_p(p);
  alm::serverstream<alm::http_processor<httpProcessor>> http_server;
  http_server.start(1100, http_p, 5000);

  websocketProcessor websocket_p;
  alm::serverstream<websocketProcessor> websocket_server;
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


