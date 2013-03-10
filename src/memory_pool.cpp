#include "alm/memory_pool.h"

namespace alm
{

memory_pool::memory_pool()
  : m_chunk_head(0), m_chunk_capacity(DEFAULT_CHUNK_CAPACITY)
{
  addChunk(m_chunk_capacity);
}

memory_pool::memory_pool(std::size_t chunkCapacity)
  : m_chunk_head(0), m_chunk_capacity(chunkCapacity)

{
  addChunk(m_chunk_capacity);
}

memory_pool::~memory_pool()
{
  clear();
}
// TODO: rename alloc as allocate
void* memory_pool::alloc(std::size_t size)
{
  if (m_chunk_head->size + size > m_chunk_head->capacity)
  {
    addChunk(m_chunk_capacity > size ? m_chunk_capacity : size);
  }
  char* buffer = (char*)(m_chunk_head + 1) + m_chunk_head->size;
  m_chunk_head->size += size;
  return buffer;
}

void memory_pool::clear()
{
  while(m_chunk_head != 0)
  {
    chunk_header* next = m_chunk_head->next;
    free(m_chunk_head);
    m_chunk_head = next;
  }
}

void memory_pool::addChunk(std::size_t capacity)
{
  chunk_header* chunk = (chunk_header*)malloc(sizeof(chunk_header) + capacity);
  chunk->capacity = capacity;
  chunk->size = 0;
  chunk->next = m_chunk_head;
  m_chunk_head = chunk;
}

const std::size_t memory_pool::DEFAULT_CHUNK_CAPACITY = 1024 * 100; // 100KB

}
