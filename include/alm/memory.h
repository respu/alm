#ifndef __ALM__MEMORY__
#define __ALM__MEMORY__

#include <cstddef> // For ptrdiff_t
#include <utility> // For forward
#include <stdexcept>

namespace alm
{

template<typename T>
inline std::size_t alignSize(std::size_t n)
{
  std::size_t alignment = std::alignment_of<T>::value - 1;
  return (((n * sizeof(T)) + alignment) & ~alignment);
}

class memory_pool
{
public:
  memory_pool()
    : m_chunk_head(0), m_chunk_capacity(DEFAULT_CHUNK_CAPACITY)
  {
    addChunk(m_chunk_capacity);
  }

  memory_pool(std::size_t chunkCapacity)
    : m_chunk_head(0), m_chunk_capacity(chunkCapacity)

  {
    addChunk(m_chunk_capacity);
  }

  ~memory_pool()
  {
    clear();
  }
// TODO: rename alloc as allocate
  void* alloc(std::size_t size)
  {
    if (m_chunk_head->size + size > m_chunk_head->capacity)
    {
      addChunk(m_chunk_capacity > size ? m_chunk_capacity : size);
    }
    char* buffer = (char*)(m_chunk_head + 1) + m_chunk_head->size;
    m_chunk_head->size += size;
    return buffer;
  }

  void clear()
  {
    while(m_chunk_head != 0)
    {
      chunk_header* next = m_chunk_head->next;
      free(m_chunk_head);
      m_chunk_head = next;
    }
  }

private:
  struct chunk_header
  {
    std::size_t   capacity;
    std::size_t   size;
    chunk_header* next;
  };

  chunk_header* m_chunk_head;
  std::size_t   m_chunk_capacity;

  void addChunk(std::size_t capacity)
  {
    chunk_header* chunk = (chunk_header*)malloc(sizeof(chunk_header) + capacity);
    chunk->capacity = capacity;
    chunk->size = 0;
    chunk->next = m_chunk_head;
    m_chunk_head = chunk;
  }

  static const std::size_t DEFAULT_CHUNK_CAPACITY = 1024 * 100; // 100KB
};

//TODO: rename as acc_allocator
template<typename T>
class allocator
{
public:
  // TODO: make it private
  memory_pool& m_pool;

  // Typedefs
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T value_type;

  /// Default constructor
  allocator(memory_pool &pool) throw()
    : m_pool(pool)
  {}
  /// Copy constructor
  allocator(const allocator &other) throw()
    : m_pool(other.m_pool)
  {
  }

  // TODO: Delete this constructor
  /// Copy constructor with another type
  template<typename U>
  allocator(const allocator<U> &other) throw()
    : m_pool(other.m_pool)
  {
  }

  /// Destructor
  ~allocator()
  {}

  /// Copy
  allocator<T>& operator=(const allocator &other)
  {
    m_pool = other.m_pool;
    return *this;
  }
  /// Copy with another type
  template<typename U>
  allocator& operator=(const allocator<U> &other) 
  {
    m_pool = other.m_pool;
    return *this;
  }

  /// Get address of reference
  pointer address(reference x) const 
  {
    return &x; 
  }
  /// Get const address of const reference
  const_pointer address(const_reference x) const 
  {
    return &x;
  }

  /// Allocate memory
  pointer allocate(size_type n, const void* = 0)
  {
    size_type size = alignSize<T>(n);
    return (pointer)m_pool.alloc(size);
  }

  /// Deallocate memory
  void deallocate(void* p, size_type n)
  {
  }

  /// Call constructor
  void construct(pointer p, const T& val)
  {
    // Placement new
    new ((T*)p) T(val); 
  }
  /// Call constructor with more arguments
  template<typename U, typename... Args>
  void construct(U* p, Args&&... args)
  {
    // Placement new
    ::new((void*)p) U(std::forward<Args>(args)...);
  }

  /// Call the destructor of p
  void destroy(pointer p) 
  {
    p->~T();
  }
  /// Call the destructor of p of type U
  template<typename U>
  void destroy(U* p)
  {
    p->~U();
  }

  /// Get the max allocation size
  size_type max_size() const 
  {
    return size_type(-1); 
  }

  /// A struct to rebind the allocator to another allocator of type U
  template<typename U>
  struct rebind
  { 
    typedef allocator<U> other; 
  };
};

template<typename T>
inline bool operator ==(const allocator<T> &,
                        const allocator<T> &)
{
  return (false);
}

template<typename T>
inline bool operator !=(const allocator<T> &, 
                        const allocator<T> &)
{
  return (true);
}

// TODO: add fixed_allocator as stack_allocator and stack_block_allocator

}

#endif
