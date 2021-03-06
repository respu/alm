#ifndef __ALM__SHA1__
#define __ALM__SHA1__

#include <string>
#include <exception>

namespace alm
{

struct sha1_zero_length_exception : std::exception {};
struct sha1_message_too_long_exception : std::exception {};

class sha1
{
public:
  static std::string digest(std::string &input);

  static std::string hexDigest(std::string &input);

private:
  static void input(const unsigned char *message_array,
                    unsigned int length);

  static std::string result();

  static std::string hexResult();

  static void process(const char* message_array);
        
  static void processMessageBlock();

  static void padMessage();

  static void reset();

  static inline unsigned int circularShift(int bits, unsigned int word);

  static unsigned int H[5];

  static unsigned int m_lengthLow;
  static unsigned int m_lengthHigh;

  static unsigned char m_messageBlock[64];
  static int m_messageBlockIndex;
};

}
#endif
