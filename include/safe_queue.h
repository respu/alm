#ifndef __ALM_SAFE_QUEUE__
#define __ALM_SAFE_QUEUE__

#include <mutex>
#include <condition_variable>
#include "queue.h"

namespace alm
{

template <typename T>
class safe_queue
{
public:
  safe_queue() {}
  ~safe_queue() {}
  safe_queue(const safe_queue &other) = delete;
  safe_queue& operator= (const safe_queue &other) = delete;

  void push(T &value)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(value);
    m_condition.notify_one(); 
  }
  
  void push(T &&value)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(std::move(value));
    m_condition.notify_one(); 
  }

  T& pop()
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_condition.wait(lock, [this]{return !m_queue.empty();});
    return m_queue.pop();
  }

private:
  alm::queue<T> m_queue;

  mutable std::mutex m_mutex;

  std::condition_variable m_condition;
};

}

#endif
