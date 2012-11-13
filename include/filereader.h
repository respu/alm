#ifndef __ALM__FILE_READER__
#define __ALM__FILE_READER__

namespace alm
{

class filereader
{

public:
  filereader(const char* fileName);
  
  ~filereader();

  unsigned int size();

  void read(char *buffer, unsigned int size);

private:
  std::ifstream m_file;

  unsigned int m_size;
};

}
#endif
