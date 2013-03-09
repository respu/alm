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
    : m_data(0), m_length(length)
  {
    reserve(m_length);
  }

  string(std::size_t length, const Allocator &&allocator)
    : m_data(0), m_length(length), m_allocator(allocator)
  {
    reserve(m_length);
  }

  string(string &&other)
    : m_data(other.m_data), m_length(other.m_length),
      m_allocator(other.m_allocator)
  {
    other.m_data = 0;
    other.m_length = 0;
  }

  ~string()
  {
    if(m_data != 0)
    {
      m_allocator.deallocate(m_data, m_length + 1);
    }
  }

  char* c_str()
  {
    return m_data;
  }

  std::size_t length()
  {
    return m_length;
  }

private:
  char* m_data;

  std::size_t m_length;

  Allocator m_allocator;

  void reserve(std::size_t length)
  {
    m_length = length; 
    m_data = (char*)m_allocator.allocate(m_length + 1);
    *(m_data + m_length) = '\0';
  }

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
    : m_pool(pool), m_head(0), m_root(0)
  {
  }

  list(list &&other)
    : m_pool(other.m_pool), m_head(other.m_head), m_root(other.m_root)
  {
    other.m_head = 0;
    other.m_root = 0;
  }

//TODO: use rvalue reference
  void push_back(T &&value)
  {
    node* new_node = (node*)m_pool.alloc(alignSize<node>(1)); 
    ::new (&(new_node->data)) T(std::move(value));
    new_node->next = 0;

    if(m_root == 0)
    {
      m_root = new_node;
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
    node* n = m_root;
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
    node* n = m_root;
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
    return m_root;
  }

private:
  memory_pool& m_pool;

  node* m_head;

  node* m_root;
};

}

#endif

