#include "alm/thread_pool.h"

namespace alm
{

join_threads::join_threads(std::vector<std::thread> &threads)
  :m_threads(threads)
{
}
  
join_threads::~join_threads()
{
  for(unsigned long i=0;i<m_threads.size();++i)
  {
    if(m_threads[i].joinable())
    {
      m_threads[i].join();
    }
  }
}

thread_pool::thread_pool(unsigned int thread_count)
  :m_running(true), m_joiner(m_threads)
{
  try
  {
    for(unsigned int i=0; i < thread_count; ++i)
    {
      m_threads.push_back(
		std::thread(&alm::thread_pool::worker_thread,this));
    }
  }
  catch(std::exception &ex)
  {
    m_running=false;
    throw ex;
  }
}
  
thread_pool::~thread_pool()
{
  m_running = false;
  clear_workers();
}

void thread_pool::worker_thread()
{
  while(m_running)
  {
    std::packaged_task<void()>& task = m_queue.pop();
    task();
  }
}

void thread_pool::clear_workers()
{
  for(unsigned long i=0;i<m_threads.size();++i)
  {
    submit(&alm::thread_pool::clean_task);
  }
}

void thread_pool::clean_task()
{
}

}
