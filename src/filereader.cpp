#include <fstream>
#include "exceptions.h"
#include "filereader.h"

namespace alm
{

filereader::filereader(const char* fileName)
{
  m_file.open(fileName, std::ios::in | std::ios::binary);
  if(!m_file.good())
  {
    throw file_not_found_exception();
  }

  m_file.seekg (0, std::ios::end);
  m_size = m_file.tellg();

  m_file.seekg (0, std::ios::beg);
}
  
filereader::~filereader()
{
  m_file.close();
}

void filereader::read(unsigned char *buffer, unsigned int size)
{
  m_file.read((char*)buffer, size);
}

unsigned int filereader::size()
{
  return m_size;
}

}
