#ifndef __ALM__MEMORY__POOL__
#define __ALM__MEMORY__POOL__

#include <cstdlib>

namespace alm
{

class memory_pool
{
public:
  memory_pool();

  memory_pool(std::size_t chunkCapacity);

  ~memory_pool();

// TODO: rename alloc as allocate
  void* alloc(std::size_t size);

  void clear();

private:
  struct chunk_header
  {
    std::size_t   capacity;
    std::size_t   size;
    chunk_header* next;
  };

  chunk_header* m_chunk_head;
  std::size_t   m_chunk_capacity;

  void addChunk(std::size_t capacity);

  static const std::size_t DEFAULT_CHUNK_CAPACITY;
};

}

#endif
