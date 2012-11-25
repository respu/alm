#ifndef __ALM_QUEUE__
#define __ALM_QUEUE__

#include <string.h>
#include <exception>

namespace alm
{

struct empty_queue_exception : std::exception {};

template <typename T>
class queue
{
public:
  queue() : m_count(0), m_capacity(DEFAULT_CAPACITY), m_front(0)
  {
    m_buffer = new T[m_capacity];
  }

  queue(const queue &other) = delete;
  queue& operator= (const queue &other) = delete;

  ~queue()
  {
    delete[] m_buffer;
  }

  void push(T &value)
  {
    m_buffer[(m_count + m_front) % m_capacity] = value;
    m_count++;

    if(m_count == m_capacity)
    {
      resize();
    }
  }

  void push(T &&value)
  {
    m_buffer[(m_count + m_front) % m_capacity] = std::move(value);
    m_count++;

    if(m_count == m_capacity)
    {
      resize();
    }
  }

  T& pop()
  {
    if(m_count == 0)
    {
      throw empty_queue_exception();
    }
    unsigned int front = m_front;
    m_front = (m_front + 1) % m_capacity;
    m_count--;
    
    return m_buffer[front];
  }

  bool empty()
  {
    return m_count == 0;
  }

private:
  static const unsigned int DEFAULT_CAPACITY = 10;

  T* m_buffer;

  unsigned int m_count;

  unsigned int m_capacity;

  unsigned int m_front;

  void resize()
  {
    m_capacity = m_count * 2;
    T* new_buffer = new T[m_capacity];
    memcpy(new_buffer, m_buffer, sizeof(T) * m_count);

    delete[] m_buffer;

    m_buffer = new_buffer;
    m_front = 0;
  }
};

}

#endif
