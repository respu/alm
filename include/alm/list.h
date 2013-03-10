#ifndef __ALM__LIST__
#define __ALM__LIST__

#include <cstring>
#include <exception>

namespace alm
{

class out_of_range : public std::exception {};

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

