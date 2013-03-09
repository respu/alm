#ifndef __ALM__CONTAINERS__
#define __ALM__CONTAINERS__

#include <stdexcept>
#include <exception>
#include "alm/memory.h"

//TODO: move here queue and safe_queue

namespace alm
{

class out_of_range : public std::exception {};

template<typename Allocator = std::allocator<char>>
class string
{
public:
  string(std::size_t length)
  {
    m_data = (char*)m_allocator.allocate(length);
  }

  string(std::size_t length, Allocator &&allocator)
    : m_allocator(allocator)
  {
    m_data = (char*)m_allocator.allocate(length);
  }

  string(string &&other)
    : m_allocator(other.m_allocator), m_data(other.m_data)
  {
    other.m_data = 0;
  }

  ~string()
  {
    m_allocator.deallocate(m_data, 1);
  }

  char* c_str()
  {
    return m_data;
  }
private:
  Allocator m_allocator;

  char* m_data;
};

template<typename K, typename V>
class pair
{
public:
  K key;

  V value;

  pair(K &&k, V &&v)
    : key(std::move(k)), value(std::move(v))
  {
  }

  pair(pair &&other)
    : key(std::move(other.key)),
      value(std::move(other.value))
  {
  }
};

//TODO: use allocator as template parameter as stl does
template<typename T>
class list
{
public:
//TODO: create iterator and keep node private
  struct node
  {
    T data;
    node* next;
  };

  list(memory_pool &pool)
    : m_pool(pool), m_head(0), m_tail(0)
  {
  }

  list(list &&other)
    : m_pool(other.m_pool), m_head(other.m_head), m_tail(other.m_tail)
  {
    other.m_head = 0;
    other.m_tail = 0;
  }

//TODO: use rvalue reference
  void push_back(T &&value)
  {
    node* new_node = (node*)m_pool.alloc(alignSize<node>(1)); 
    ::new (&(new_node->data)) T(std::move(value));
    new_node->next = 0;

    if(m_tail == 0)
    {
      m_tail = new_node;
      m_head = new_node;
    }
    else
    {
      m_head->next = new_node;
      m_head = new_node;
    }
  }

  T& at(std::size_t pos)
  {
    node* n = m_tail;
    std::size_t index = 0;
    while(n)
    {
      if(index == pos)
      {
        return n->data;
      }
      n = n->next;
      index++;
    }
    throw out_of_range();
  } 

  std::size_t size()
  {
    node* n = m_tail;
    std::size_t index = 0;
    while(n)
    {
      n = n->next;
      index++;
    }
    return index;
  }

  node* begin()
  {
    return m_head;
  }

  node* end()
  {
    return m_tail;
  }

private:
  memory_pool& m_pool;

  node* m_head;

  node* m_tail;
};

}

#endif

