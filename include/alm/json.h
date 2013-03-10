#ifndef __ALM__json___
#define __ALM__json___

#include <sstream>
#include <exception>
#include <cassert>
#include <cstring>
#include "alm/memory_pool.h"
#include "alm/acc_allocator.h"
#include "alm/list.h"
#include "alm/string.h"

namespace alm
{

struct json_exception : public std::exception {};
struct json_object_map_exception : public std::exception {};

class json_array;
class json_object;
class json_value;

typedef string<acc_allocator<char>> json_string;

typedef node<json_value> json_list_node;
typedef list<json_value, acc_allocator<json_list_node>> json_list;

typedef pair<json_string, json_value> json_pair;
typedef node<json_pair> json_map_node;
typedef list<json_pair, acc_allocator<json_map_node>> json_map;

class json
{
public:
  static void check(std::stringstream &input, const char* pattern);

  static bool match(std::stringstream &input, const char* pattern);
  
  static std::size_t stringLength(std::stringstream &input);

  static void fillString(std::stringstream &input, json_string &s);

  static json_string* createString(std::size_t length, memory_pool &pool);

  static json_array* createArray(memory_pool &pool);

  static json_object* createObject(memory_pool &pool);
};

class json_value
{
public:
  json_value();

  ~json_value();

  void deserialize(std::stringstream &input, memory_pool &pool);

  void serialize(std::stringstream &output);

  template<typename T>
  T& get();

  template<typename T>
  bool is();

  template<typename T>
  void put(T&& value, memory_pool &pool);

  void putNull();

private:
  void parseNull(std::stringstream &input);

  void parseBool(std::stringstream &input);

  void parseNumber(std::stringstream &input);

  void parseString(std::stringstream &input, memory_pool &pool);

  void parseArray(std::stringstream &input, memory_pool &pool);

  void parseObject(std::stringstream &input, memory_pool &pool);

  enum
  {
    JSON_UNINIT,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_NULL,
    JSON_ARRAY,
    JSON_OBJECT
  } m_type;

  union
  {
    bool         m_bool;
    double       m_number;
    json_string* m_string;
    json_array*  m_array;
    json_object* m_object;
  };
};

class json_array
{
public:
  json_array(memory_pool &pool);

  json_array(json_array &&other);

  ~json_array();

  void deserialize(std::stringstream &input);

  void serialize(std::stringstream &output);

  size_t size()
  {
    return m_values.size();
  }

  template<typename T>
  bool has(unsigned int index)
  {
    assert(m_values.size() > index);

    return m_values.at(index).is<T>();
  }

  template<typename T>
  T& get(unsigned int index)
  {
    assert(m_values.size() > index);

    return m_values.at(index).get<T>();
  }

  template<typename T>
  void put(T &&value)
  {
    json_value v;
    v.put<T>(std::move(value), m_pool);
    m_values.push_back(std::move(v));
  }

  void putNull()
  {
    json_value v;
    v.putNull(); 
    m_values.push_back(std::move(v));
  }

private:
  memory_pool& m_pool;

  json_list m_values;
};

class json_object
{
public:
  json_object(memory_pool &pool);

  json_object(json_object &&other);

  ~json_object();

  void deserialize(std::stringstream &input);

  void serialize(std::stringstream &output);

  size_t size()
  {
    return m_values.size();
  }

  template<typename T>
  bool has(const char* key)
  {
    bool result = false;
    json_map_node* n = m_values.begin();
    while(n)
    {
      if(strcmp(key, n->data.key.c_str()) == 0)
      {
        result = true;
        n = 0;
      }
      n = n->next;
    }
    return result;
  }

  template<typename T>
  T& get(const char* key)
  {
    json_map_node* n = m_values.begin();
    while(n)
    {
      if(strcmp(key, n->data.key.c_str()) == 0)
      {
        return n->data.value.get<T>();
      }
      n = n->next;
    }
    throw json_object_map_exception();
  }

  template<typename T>
  void put(const char* key, T &&value)
  {
    json_string key_string(strlen(key), acc_allocator<char>(m_pool));
    strcpy(key_string.c_str(), key); 

    json_value v;
    v.put<T>(std::move(value), m_pool);
    m_values.push_back(std::move(json_pair(std::move(key_string), std::move(v))));
  }

  void putNull(const char* key)
  {
    json_string key_string(strlen(key), acc_allocator<char>(m_pool));
    strcpy(key_string.c_str(), key); 

    json_value v;
    v.putNull();
    m_values.push_back(std::move(json_pair(std::move(key_string), std::move(v))));
  }

private:
  memory_pool& m_pool;

  json_map m_values;
};

class json_document
{
public:
  json_document();

  json_document(std::size_t size);

  ~json_document();

  void deserialize(std::stringstream &input);

  void serialize(std::stringstream &output);

  memory_pool& pool();

  json_object& root();

private:
  memory_pool m_pool;

  json_object m_root;
};

template<>
inline bool json_value::is<bool>()
{
  return m_type == JSON_BOOL;
}

template<>
inline bool json_value::is<json_string>()
{
  return m_type == JSON_STRING;
}

template<>
inline bool json_value::is<double>()
{
  return m_type == JSON_NUMBER;
}

template<>
inline bool json_value::is<json_array>()
{
  return m_type == JSON_ARRAY;
}

template<>
inline bool json_value::is<json_object>()
{
  return m_type == JSON_OBJECT;
}

template<>
inline bool& json_value::get<bool>()
{
  assert(is<bool>());

  return m_bool;
}

template<>
inline json_string& json_value::get<json_string>()
{
  assert(is<json_string>());

  return *m_string;
}

template<>
inline double& json_value::get<double>()
{
  assert(is<double>());

  return m_number;
}

template<>
inline json_array& json_value::get<json_array>()
{
  assert(is<json_array>());

  return *m_array;
}

template<>
inline json_object& json_value::get<json_object>()
{
  assert(is<json_object>());

  return *m_object;
}

template<>
inline void json_value::put<bool>(bool &&value, memory_pool &pool)
{
  assert(m_type == JSON_UNINIT);

  m_type = JSON_BOOL;
  m_bool = value;
}

template<>
inline void json_value::put<double>(double &&value, memory_pool &pool)
{
  assert(m_type == JSON_UNINIT);

  m_type = JSON_NUMBER;
  m_number = value;
}

template<>
inline void json_value::put<json_string>(json_string &&value, memory_pool &pool)
{
  assert(m_type == JSON_UNINIT);

  m_type = JSON_STRING;
  m_string = (json_string*)pool.alloc(alignSize<json_string>(1));
  :: new (m_string) json_string(std::move(value));
}

template<>
inline void json_value::put<json_array>(json_array &&value, memory_pool &pool)
{
  assert(m_type == JSON_UNINIT);

  m_type = JSON_ARRAY;
  m_array = (json_array*)pool.alloc(alignSize<json_array>(1));
  :: new (m_array) json_array(std::move(value));
}

template<>
inline void json_value::put<json_object>(json_object &&value, memory_pool &pool)
{
  assert(m_type == JSON_UNINIT);

  m_type = JSON_OBJECT;
  m_object = (json_object*)pool.alloc(alignSize<json_object>(1));
  :: new (m_object) json_object(std::move(value));
}

}
#endif
