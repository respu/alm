#ifndef __ALM__STACK__BLOCK__ALLOCATOR__
#define __ALM__STACK__BLOCK__ALLOCATOR__

#include <stdexcept>
#include <algorithm>
#include <functional>
#include <type_traits>

namespace alm
{

template <typename T, const size_t MAX_SIZE>
class stack_block_allocator
{
public:
typedef T                  value_type;
  typedef value_type *       pointer;
  typedef const value_type * const_pointer;
  typedef value_type &       reference;
  typedef const value_type & const_reference;
  typedef std::size_t        size_type;
  typedef std::ptrdiff_t     difference_type;

  enum
  {
    // The numbers of buffers to use. Varies according to the type.
    NUMBER_OF_BUFFERS = std::has_trivial_destructor<T>::value ? 1 : 2
  };

  template<typename U>
  struct rebind
  {
      typedef stack_block_allocator<U, MAX_SIZE> other;
  };

  stack_block_allocator()
      : buffer_id(0)
  {
      initialise();
  }

  stack_block_allocator(const stack_block_allocator &rhs)
      : buffer_id(0)
  {
      initialise();
  }

  template<typename U>
  stack_block_allocator(const stack_block_allocator<U, MAX_SIZE> &rhs)
      : buffer_id(0)
  {
      initialise();
  }

  ~stack_block_allocator()
  {
  }

  pointer address(reference x) const
  {
      return (&x);
  }

  const_pointer address(const_reference x) const
  {
      return (x);
  }

  //*********************************************************************
  // allocate
  // Allocates from the internal array.
  // If storage cannot be allocated then std::bad_alloc() is thrown.
  //*********************************************************************
  pointer allocate(size_type     n,
                   const_pointer cp = 0)
  {
      // Just too big?
      if (n > MAX_SIZE)
      {
          throw std::bad_alloc();
      }

      // Get the next buffer.
      buffer_id = ++buffer_id % NUMBER_OF_BUFFERS;

      // Always return the beginning of the buffer.
      return (reinterpret_cast<pointer>(p_buffer[buffer_id]));
  }

  void deallocate(pointer   p,
                  size_type n)
  {
  }

  //*********************************************************************
  // max_size
  // Returns the maximum size that can be allocated in total.
  //*********************************************************************
  size_type max_size() const
  {
      return (MAX_SIZE);
  }

  void construct(pointer          p,
                 const value_type &x)
  {
      new (p)value_type(x);
  }

  void destroy(pointer p)
  {
      p->~value_type();
  }

private:

  enum
  {
      ALIGNMENT = std::alignment_of<T>::value - 1 // The alignment of the buffers - 1
  };

  void initialise()
  {
      // Ensure alignment.
      for (int i = 0; i < NUMBER_OF_BUFFERS; ++i)
      {
          p_buffer[i] = reinterpret_cast<char *>((reinterpret_cast<size_t>(&buffer[i][0]) + ALIGNMENT) & ~ALIGNMENT);
      }
  }

  // Disabled operator.
  void operator =(const stack_block_allocator &);

  // The allocation buffers. Ensure enough space for correct alignment.
  char buffer[NUMBER_OF_BUFFERS][(MAX_SIZE * sizeof(value_type)) + ALIGNMENT + 1];

  // Pointers to the first valid locations in the buffers after alignment.
  char *p_buffer[NUMBER_OF_BUFFERS];

  // The index of the currently allocated buffer.
  int buffer_id;
};

template<typename T, const size_t MAX_SIZE>
inline bool operator ==(const stack_block_allocator<T, MAX_SIZE> &,
                        const stack_block_allocator<T, MAX_SIZE> &)
{
    return (false);
}

template<typename T, const size_t MAX_SIZE>
inline bool operator !=(const stack_block_allocator<T, MAX_SIZE> &,
                        const stack_block_allocator<T, MAX_SIZE> &)
{
    return (true);
}

}

#endif
