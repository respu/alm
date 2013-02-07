#ifndef __ALM__json___
#define __ALM__json___

#include <vector>
#include <map>
#include <sstream>
#include <exception>

namespace alm
{

struct json_exception : public std::exception {};

class json_array;
class json_object;

class json_value
{
public:
  ~json_value();

  void deserialize(std::stringstream &input);

  void serialize(std::stringstream &output);

  template<typename T>
  T& get();

  template<typename T>
  void put(T&& value);

private:
  void parseNull(std::stringstream &input);

  void parseBool(std::stringstream &input);

  void parseNumber(std::stringstream &input);

  void parseString(std::stringstream &input);

  void parseArray(std::stringstream &input);

  void parseObject(std::stringstream &input);

  enum
  {
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
    std::string* m_string;
    json_array*  m_array;
    json_object* m_object;
  };
};

class json_array
{
public:
  json_array();

  json_array(json_array &&other);

  ~json_array();

  void deserialize(std::stringstream &input);

  void serialize(std::stringstream &output);

  size_t size()
  {
    return m_values->size();
  }

  template<typename T>
  T& get(unsigned int index)
  {
    json_value* value = m_values->at(index);
    return value->get<T>();
  }

  template<typename T>
  void put(T &&value)
  {
    json_value* v = new json_value();
    v->put<T>(std::move(value));
    m_values->push_back(v);
  }

private:
  std::vector<json_value*>* m_values;
};

class json_object
{
public:
  json_object();

  json_object(json_object &&other);

  ~json_object();

  void deserialize(std::stringstream &input);

  void serialize(std::stringstream &output);

  size_t size()
  {
    return m_values->size();
  }

  template<typename T>
  T& get(const char* key)
  {
    json_value* value = m_values->at(key);
    return value->get<T>();
  }

  template<typename T>
  void put(const char* key, T &&value)
  {
    json_value* v = new json_value();
    v->put<T>(std::move(value));
    m_values->insert(std::pair<std::string,json_value*>(key,v));
  }

private:
  std::map<std::string, json_value*>* m_values;
};

class json
{
public:
  static void check(std::stringstream &input, const char* pattern);

  static bool match(std::stringstream &input, const char* pattern);
  
  static void read(std::stringstream &input, std::string &str);
};

template<>
inline bool& json_value::get<bool>()
{
  return m_bool;
}

template<>
inline std::string& json_value::get<std::string>()
{
  return *m_string;
}

template<>
inline double& json_value::get<double>()
{
  return m_number;
}

template<>
inline json_array& json_value::get<json_array>()
{
  return *m_array;
}

template<>
inline json_object& json_value::get<json_object>()
{
  return *m_object;
}

template<>
inline void json_value::put<bool>(bool &&value)
{
  m_type = JSON_BOOL;
  m_bool = value;
}

template<>
inline void json_value::put<double>(double &&value)
{
  m_type = JSON_NUMBER;
  m_number = value;
}

template<>
inline void json_value::put<std::string>(std::string &&value)
{
  m_type = JSON_STRING;
  m_string = new std::string(std::move(value));
}

template<>
inline void json_value::put<json_array>(json_array &&value)
{
  m_type = JSON_ARRAY;
  m_array = new json_array(std::move(value));
}

template<>
inline void json_value::put<json_object>(json_object &&value)
{
  m_type = JSON_OBJECT;
  m_object = new json_object(std::move(value));
}

}
#endif
