#include "filewriter.h"

alm::filewriter::filewriter(const char* fileName)
{
  m_file.open(fileName, std::ios::out | std::ios::binary);
}
  
alm::filewriter::~filewriter()
{
  m_file.close();
}

void alm::filewriter::write(const char* buffer, int size)
{
  m_file.write(buffer, size); 
}
