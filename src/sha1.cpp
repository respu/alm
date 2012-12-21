#include <string.h>
#include <string>
#include <sstream>
#include "sha1.h"
#include "endianess.h"

namespace alm
{

unsigned int sha1::H[5] =
   {
    0x67452301,
    0xEFCDAB89,
    0x98BADCFE,
    0x10325476,
    0xC3D2E1F0 
   };

unsigned int sha1::m_lengthLow = 0;
unsigned int sha1::m_lengthHigh = 0;

unsigned char sha1::m_messageBlock[64];
int sha1::m_messageBlockIndex = 0;

void sha1::reset()
{
    H[0]=0x67452301;
    H[1]=0xEFCDAB89;
    H[2]=0x98BADCFE;
    H[3]=0x10325476;
    H[4]=0xC3D2E1F0; 

m_lengthLow = 0;
m_lengthHigh = 0;

m_messageBlockIndex = 0;
}

void sha1::input(const unsigned char *message_array,
                 unsigned int length)
{
    if (!length)
    {
        throw sha1_zero_length_exception();
    }

    while(length--)
    {
        m_messageBlock[m_messageBlockIndex++] = (*message_array & 0xFF);

        m_lengthLow += 8;
        m_lengthLow &= 0xFFFFFFFF;               // Force it to 32 bits
        if (m_lengthLow == 0)
        {
            m_lengthHigh++;
            m_lengthHigh &= 0xFFFFFFFF;          // Force it to 32 bits
            if (m_lengthHigh == 0)
            {
                throw sha1_message_too_long_exception();
            }
        }

        if (m_messageBlockIndex == 64)
        {
            processMessageBlock();
        }

        message_array++;
    }
}

void sha1::process(const char* message_array)
{
    const unsigned char* p = (const unsigned char*)message_array;

    reset();

    while(*p)
    {
        input(p, 1);
        p++;
    }
}

std::string sha1::digest(std::string &input)
{
  process(input.c_str());
  return result();
}

std::string sha1::hexDigest(std::string &input)
{
  process(input.c_str());
  return hexResult();
}

void sha1::processMessageBlock()
{
    const unsigned int K[] = {
                                0x5A827999,
                                0x6ED9EBA1,
                                0x8F1BBCDC,
                                0xCA62C1D6
                             };
    int          t;
    unsigned int temp;
    unsigned int W[80];
    unsigned int A, B, C, D, E;

    for(t = 0; t < 16; t++)
    {
        W[t] = ((unsigned) m_messageBlock[t * 4]) << 24;
        W[t] |= ((unsigned) m_messageBlock[t * 4 + 1]) << 16;
        W[t] |= ((unsigned) m_messageBlock[t * 4 + 2]) << 8;
        W[t] |= ((unsigned) m_messageBlock[t * 4 + 3]);
    }

    for(t = 16; t < 80; t++)
    {
       W[t] = circularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }

    A = H[0];
    B = H[1];
    C = H[2];
    D = H[3];
    E = H[4];

    for(t = 0; t < 20; t++)
    {
        temp = circularShift(5,A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = circularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 20; t < 40; t++)
    {
        temp = circularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = circularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 40; t < 60; t++)
    {
        temp = circularShift(5,A) +
               ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = circularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 60; t < 80; t++)
    {
        temp = circularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = circularShift(30,B);
        B = A;
        A = temp;
    }

    H[0] = (H[0] + A) & 0xFFFFFFFF;
    H[1] = (H[1] + B) & 0xFFFFFFFF;
    H[2] = (H[2] + C) & 0xFFFFFFFF;
    H[3] = (H[3] + D) & 0xFFFFFFFF;
    H[4] = (H[4] + E) & 0xFFFFFFFF;

    m_messageBlockIndex = 0;
}

void sha1::padMessage()
{
    if (m_messageBlockIndex > 55)
    {
        m_messageBlock[m_messageBlockIndex++] = 0x80;
        while(m_messageBlockIndex < 64)
        {
            m_messageBlock[m_messageBlockIndex++] = 0;
        }

        processMessageBlock();

        while(m_messageBlockIndex < 56)
        {
            m_messageBlock[m_messageBlockIndex++] = 0;
        }
    }
    else
    {
        m_messageBlock[m_messageBlockIndex++] = 0x80;
        while(m_messageBlockIndex < 56)
        {
            m_messageBlock[m_messageBlockIndex++] = 0;
        }

    }

    m_messageBlock[56] = (m_lengthHigh >> 24) & 0xFF;
    m_messageBlock[57] = (m_lengthHigh >> 16) & 0xFF;
    m_messageBlock[58] = (m_lengthHigh >> 8) & 0xFF;
    m_messageBlock[59] = (m_lengthHigh) & 0xFF;
    m_messageBlock[60] = (m_lengthLow >> 24) & 0xFF;
    m_messageBlock[61] = (m_lengthLow >> 16) & 0xFF;
    m_messageBlock[62] = (m_lengthLow >> 8) & 0xFF;
    m_messageBlock[63] = (m_lengthLow) & 0xFF;

    processMessageBlock();
}

unsigned int sha1::circularShift(int bits, unsigned int word)
{
    return ((word << bits) & 0xFFFFFFFF) | ((word & 0xFFFFFFFF) >> (32-bits));
}

std::string sha1::result()
{
    padMessage();

    unsigned char buffer[sizeof(H)];
    memcpy(buffer, H, sizeof(H));
    return std::string((const char*)buffer, sizeof(buffer));
}

std::string sha1::hexResult()
{
    padMessage();

    std::stringstream ss;
    std::ios::fmtflags flags = 
      ss.setf(std::ios::hex|std::ios::uppercase,std::ios::basefield);
    ss.setf(std::ios::uppercase);
    for(int i = 0; i < 5 ; i++)
    {
        ss << H[i];
    } 
    ss.setf(flags);
    return ss.str();
}

}
