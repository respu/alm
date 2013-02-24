#ifndef __ALM__SINGLETON__
#define __ALM__SINGLETON__

#include <memory>
#include <mutex>

namespace alm
{

template<typename T>
class singleton
{
public:
  static T& instance()
  {
    return *(m_instance ? m_instance : (m_instance = new T()));
  }
 
private:
  static T* m_instance;

  singleton()                                = delete;
  ~singleton()                               = delete;
  singleton(const singleton& src)            = delete;
  singleton& operator=(const singleton& rhs) = delete;
};

template<typename T>
T* singleton<T>::m_instance = nullptr;

template<typename T>
class safe_singleton
{

public:
  static T& instance()
  {
    std::call_once(m_onceFlag, []
      {
	m_instance.reset(new T());
      });
    return *m_instance.get();
  }
 
private:
  static std::unique_ptr<T> m_instance;
  static std::once_flag     m_onceFlag;

  safe_singleton()                                     = delete;
  ~safe_singleton()                                    = delete;
  safe_singleton(const safe_singleton& src)            = delete;
  safe_singleton& operator=(const safe_singleton& rhs) = delete;
};

template<typename T>
std::unique_ptr<T> safe_singleton<T>::m_instance = nullptr;

template<typename T>
std::once_flag     safe_singleton<T>::m_onceFlag;

}
#endif
