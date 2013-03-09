#ifndef __ALM__CONTAINERS__
#define __ALM__CONTAINERS__

#include <cstring>
#include <exception>
#include "alm/memory.h"

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

  string(const char* value)
    : m_data(0), m_length(0)
  {
    reserve(strlen(value));

    strcpy(m_data, value);
  }

  string(std::size_t length, Allocator &&allocator)
    : m_data(0), m_length(length), m_allocator(allocator)
  {
    reserve(m_length);
  }

  string(const char* value, Allocator &&allocator)
    : m_data(0), m_length(0), m_allocator(allocator)
  {
    reserve(strlen(value));

    strcpy(m_data, value);
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
    // Add \0 character at the end of the string to make
    // it printable by std::cout
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

template<typename T>
struct node
{
  T data;
  node* next;

  node(T &&other_data)
    : data(std::move(other_data)), next(0)
  {
  }
};

template<typename T, typename Allocator = std::allocator<node<T>>>
class list
{
public:
  list()
    : m_head(0), m_root(0)
  {
  }

  list(Allocator &&allocator)
    : m_allocator(allocator), m_head(0), m_root(0)
  {
  }

  list(list &&other)
    : m_allocator(other.m_allocator), m_head(other.m_head), m_root(other.m_root)
  {
    other.m_head = 0;
    other.m_root = 0;
  }

  ~list()
  {
    node<T>* n = m_root;
    while(n)
    {
      node<T>* tmp = n;
      n = n->next;

      m_allocator.destroy(tmp);
      m_allocator.deallocate(tmp, 1);
    }    
  }

  void push_back(T &&value)
  {
    node<T>* new_node = m_allocator.allocate(1); 
    m_allocator.construct(new_node, std::move(value));

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

//TODO: implement erase

  T& at(std::size_t pos)
  {
    node<T>* n = m_root;
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
    node<T>* n = m_root;
    std::size_t index = 0;
    while(n)
    {
      n = n->next;
      index++;
    }
    return index;
  }

  node<T>* begin()
  {
    return m_root;
  }

private:
  Allocator m_allocator;

  node<T>* m_head;

  node<T>* m_root;
};

}

#endif

