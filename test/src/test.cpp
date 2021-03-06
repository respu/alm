#include <iostream>
#include <vector>
#include <sstream>
#include "alm/thread_pool.h"
#include "alm/ibstream.h"
#include "alm/obstream.h"
#include "alm/tcp_server.h"
#include "alm/tcp_client.h"
#include "alm/safe_map.h"
#include "alm/endianess.h"
#include "alm/sha1.h"
#include "alm/base64.h"

using namespace std;

#define TESTA   "abc"
#define TESTB   "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
#define TESTC   "V1teFrO0hH+LmO5hEoITfg==258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

struct processor
{
  void onOpen(int newSocketFD, sockaddr_in clientAddr) { }

  void onClose(int socketFD) { }

  void onMessage(int socketFD) { }
};

void f1()
{
  alm::thread_pool pool(2);
  alm::ibstream input;
  alm::obstream output;

  alm::tcp_server<processor> server;

  alm::tcp_client<processor> client;

  alm::safe_map<int, int> map;
}

void DisplayMessageDigest(unsigned *message_digest)
{
    ios::fmtflags   flags, flags2;

    cout << '\t';

    flags = cout.setf(ios::hex|ios::uppercase,ios::basefield);
    cout.setf(ios::uppercase);

    for(int i = 0; i < 5 ; i++)
    {
        cout << message_digest[i] << ' ';
    }

    cout << endl;

    cout.setf(flags);

    std::stringstream ss;
    flags2 = ss.setf(ios::hex|ios::uppercase,ios::basefield);
    ss.setf(ios::uppercase);
    for(int i = 0; i < 5 ; i++)
    {
        ss << message_digest[i];
    } 
    ss.setf(flags2);

    cout << "ss: " << ss.str() << std::endl;
}

void testSHA1()
{

  {
    /*
     *  Perform test A
     */
    cout << endl << "Test A: 'abc'" << endl;

    std::string input(TESTA);
    std::string result = alm::sha1::hexDigest(input);
    std::cout << "result: " << result << std::endl;
    cout << "Should match:" << endl;
    cout << '\t' << "A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D" << endl;
  }

  {
    /*
     *  Perform test B
     */
    cout << endl << "Test B: " << TESTB << endl;

    std::string input(TESTB);
    std::string result = alm::sha1::hexDigest(input);
    std::cout << "result: " << result << std::endl;
    cout << "Should match:" << endl;
    cout << '\t' << "84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1" << endl;
  }

  {
    /*
     *  Perform test C
     */
    cout << endl << "Test C: " << TESTC << endl;

    std::string input(TESTC);
    std::string hexresult = alm::sha1::hexDigest(input);
    std::cout << "hexresult: " << hexresult << std::endl;

    std::string result = alm::sha1::digest(input);
    std::cout << "result: " << result << std::endl;

    std::string hashedKey = alm::base64::encode(result);
    std::cout << "hashedKey: " << hashedKey << std::endl;
  }
}

int testBase64()
{
    std::vector<std::string> decoded =
      {
        "pleasure."
      , "leasure."
      , "easure."
      , "asure."
      , "sure."
      };

    std::vector<std::string> encoded =
      {
        "cGxlYXN1cmUu"
      , "bGVhc3VyZS4="
      , "ZWFzdXJlLg=="
      , "YXN1cmUu"
      , "c3VyZS4="
      };

    {
      std::vector<std::string>::iterator it = decoded.begin();
      for (; it != decoded.end(); ++it)
      {
        std::string result = alm::base64::encode(*it);

        std::cout << "Decoded: " << *it << " Encoded: " << result << std::endl << std::endl;
      }
    }

    {
      std::vector<std::string>::iterator it = encoded.begin();
      for (; it != encoded.end(); ++it)
      {
        std::string result = alm::base64::decode(*it);

        std::cout << "Encoded: " << *it << " Decoded: " << result << std::endl << std::endl;
      }
    }

    return 0;
}

int main()
{
  testSHA1();
}
