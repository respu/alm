#ifndef __ALM_SAFE_MAP__
#define __ALM_SAFE_MAP__

#include <mutex>
#include <condition_variable>
#include <map>
#include <exception>

namespace alm
{

struct not_found_exception : std::exception {};

template <typename K, typename V>
class safe_map
{
public:
  safe_map() {}
  ~safe_map() {}
  safe_map(const safe_map &other) = delete;
  safe_map& operator= (const safe_map &other) = delete;

  void insert(K &key, V &value)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_map[key] = value;
  }
  
  V find(K &key)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    typename std::map<K,V>::iterator it = m_map.find(key);
    if(it == m_map.end())
    {
      throw not_found_exception();
    }
    return it->second;
  }

  template<typename FunctionType>
  void erase(K &key, FunctionType f)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    typename std::map<K,V>::iterator it = m_map.find(key);
    if(it == m_map.end())
    {
      throw not_found_exception();
    }
    f(it->second);
    m_map.erase(key);
  }

  template<typename FunctionType>
  void for_each(FunctionType f)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    typename std::map<K,V>::iterator it = m_map.begin();
    for(; it != m_map.end(); ++it)
    {
      f(it->second);
    } 
  }

private:
  std::map<K,V> m_map;

  mutable std::mutex m_mutex;
};

}

#endif
