#ifndef __ALM__sha1__
#define __ALM__sha1__

#include <exception>

namespace alm
{

struct sha1_zero_length_exception : std::exception {};
struct sha1_message_too_long_exception : std::exception {};

class sha1
{
public:
  static void encode(const char* message_array, unsigned* output);

private:
  static void input(const unsigned char *message_array,
                    unsigned length);

  static void result(unsigned *message_digest_array);
        
  static void processMessageBlock();

  static void padMessage();

  static void reset();

  static inline unsigned circularShift(int bits, unsigned word);

  static unsigned H[5];

  static unsigned m_lengthLow;
  static unsigned m_lengthHigh;

  static unsigned char m_messageBlock[64];
  static int m_messageBlockIndex;
};

}
#endif