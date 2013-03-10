#ifndef __ALM__ACC__ALLOCATOR__
#define __ALM__ACC__ALLOCATOR__

#include <cstddef> // For ptrdiff_t
#include <utility> // For forward
#include "alm/memory_pool.h"

namespace alm
{

template<typename T>
inline std::size_t alignSize(std::size_t n)
{
  std::size_t alignment = std::alignment_of<T>::value - 1;
  return (((n * sizeof(T)) + alignment) & ~alignment);
}

template<typename T>
class acc_allocator
{
public:
  // Needed to be public by constructor with acc_allocator<U>
  // That constructor is used by std::map
  memory_pool& m_pool;

  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T value_type;

  acc_allocator(memory_pool &pool) throw()
    : m_pool(pool)
  {
  }

  acc_allocator(const acc_allocator &other) throw()
    : m_pool(other.m_pool)
  {
  }

  template<typename U>
  acc_allocator(const acc_allocator<U> &other) throw()
    : m_pool(other.m_pool)
  {
  }

  ~acc_allocator()
  {
  }

  acc_allocator<T>& operator=(const acc_allocator &other)
  {
    m_pool = other.m_pool;
    return *this;
  }

  template<typename U>
  acc_allocator& operator=(const acc_allocator<U> &other) 
  {
    m_pool = other.m_pool;
    return *this;
  }

  pointer address(reference x) const 
  {
    return &x; 
  }

  const_pointer address(const_reference x) const 
  {
    return &x;
  }

  pointer allocate(size_type n, const void* = 0)
  {
    size_type size = alignSize<T>(n);
    return (pointer)m_pool.alloc(size);
  }

  void deallocate(void* p, size_type n)
  {
  }

  void construct(pointer p, const T& val)
  {
    new ((T*)p) T(val); 
  }

  template<typename U, typename... Args>
  void construct(U* p, Args&&... args)
  {
    ::new((void*)p) U(std::forward<Args>(args)...);
  }

  void destroy(pointer p) 
  {
    p->~T();
  }

  template<typename U>
  void destroy(U* p)
  {
    p->~U();
  }

  size_type max_size() const 
  {
    return size_type(-1); 
  }

  template<typename U>
  struct rebind
  { 
    typedef acc_allocator<U> other; 
  };
};

template<typename T>
inline bool operator ==(const acc_allocator<T> &,
                        const acc_allocator<T> &)
{
  return (false);
}

template<typename T>
inline bool operator !=(const acc_allocator<T> &, 
                        const acc_allocator<T> &)
{
  return (true);
}

}

#endif
