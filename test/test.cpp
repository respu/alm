#include <iostream>
#include <vector>
#include "thread_pool.h"
#include "ibstream.h"
#include "obstream.h"
#include "serverstream.h"
#include "clientstream.h"
#include "safe_map.h"
#include "endianess.h"
#include "sha1.h"
#include "base64.h"

using namespace std;

#define TESTA   "abc"
#define TESTB   "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"


struct processor
{
  void addClient(int newSocketFD, sockaddr_in clientAddr) { }

  void removeClient(int socketFD) { }

  void recvMessage(int socketFD) { }
};

void f1()
{
  alm::thread_pool pool(2);
  alm::ibstream input(5);
  alm::obstream output;

  processor p;
  alm::serverstream<processor> server(1100, p);

  alm::clientstream client;

  alm::safe_map<int, int> map;
}

void DisplayMessageDigest(unsigned *message_digest)
{
    ios::fmtflags   flags;

    cout << '\t';

    flags = cout.setf(ios::hex|ios::uppercase,ios::basefield);
    cout.setf(ios::uppercase);

    for(int i = 0; i < 5 ; i++)
    {
        cout << message_digest[i] << ' ';
    }

    cout << endl;

    cout.setf(flags);
}

void testSHA1()
{

  {
    unsigned    message_digest[5];

    /*
     *  Perform test A
     */
    cout << endl << "Test A: 'abc'" << endl;

    alm::sha1::encode(TESTA, message_digest);

        DisplayMessageDigest(message_digest);
        cout << "Should match:" << endl;
        cout << '\t' << "A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D" << endl;
  }

  {
    unsigned    message_digest[5];


    /*
     *  Perform test B
     */
    cout << endl << "Test B: " << TESTB << endl;

    alm::sha1::encode(TESTB, message_digest);

        DisplayMessageDigest(message_digest);
        cout << "Should match:" << endl;
        cout << '\t' << "84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1" << endl;
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
  testBase64();
}
