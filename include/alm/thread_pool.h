#ifndef __ALM_THREAD_POOL__
#define __ALM_THREAD_POOL__

#include <thread>
#include <vector>
#include <atomic>
#include <future>
#include "alm/safe_queue.h"

namespace alm
{

class join_threads
{
public:
  explicit join_threads(std::vector<std::thread> &threads);
  
  ~join_threads();

private:
  std::vector<std::thread>& m_threads;
};

class thread_pool
{
public:
  thread_pool(unsigned int thread_count);
  
  ~thread_pool();

  template<typename FunctionType>
  std::future<void> submit(FunctionType f)
  {
    std::packaged_task<void()> task(std::move(f));
    std::future<void> res(task.get_future());
    m_queue.push(std::move(task));
    return res;
  }

private:
  alm::safe_queue<std::packaged_task<void()>> m_queue;

  std::atomic_bool m_running;

  std::vector<std::thread> m_threads;

  join_threads m_joiner;

  void worker_thread();

  void clear_workers();

  static void clean_task();
};

}

#endif
