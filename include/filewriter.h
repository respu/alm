#ifndef __ALM__FILE_WRITER__
#define __ALM__FILE_WRITER__

#include <fstream>

namespace alm
{

class filewriter
{

public:
  filewriter(const char* fileName);
  
  ~filewriter();

  void write(const char* buffer, int size);

private:
  std::ofstream m_file;
};

}
#endif
