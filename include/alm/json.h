#ifndef __ALM__json___
#define __ALM__json___

#include <vector>
#include <map>
#include <sstream>
#include <exception>

namespace alm
{

struct json_exception : public std::exception {};

class json_value
{
public:
  json_value(){}

  virtual ~json_value(){}

  virtual void parse(std::stringstream &input)    = 0;
};

class json_number : public json_value 
{
public:
  json_number(){}

  virtual ~json_number(){}

  virtual void parse(std::stringstream &input);

private:
  double m_data;
};

class json_string : public json_value
{
public:
  json_string(){}

  virtual ~json_string(){}

  virtual void parse(std::stringstream &input);

private:
  std::string m_data;
};

class json_bool : public json_value
{
public:
  json_bool(){}

  virtual ~json_bool(){}

  virtual void parse(std::stringstream &input);

private:
  bool m_data;
};

class json_null : public json_value
{
public:
  json_null(){}

  virtual ~json_null(){}

  virtual void parse(std::stringstream &input);
};

class json_array : public json_value
{
public:
  json_array(){}

  virtual ~json_array();

  virtual void parse(std::stringstream &input);

private:
  std::vector<json_value*> m_values;
};

class json_object : public json_value
{
public:
  json_object(){}

  virtual ~json_object();

  virtual void parse(std::stringstream &input);
  
private:
  std::map<std::string, json_value*> m_values;
};

class json
{
public:
  static void check(std::stringstream &input, const char* pattern);

  static bool match(std::stringstream &input, const char* pattern);
  
  static void read(std::stringstream &input, std::string &str);

  static json_value* parse(std::stringstream &input);
};

class json_holder
{
public:
  json_holder();

  ~json_holder();

  void operator<<(std::stringstream &input);

private:
  json_value* m_value;
};

class json_visitor
{
};

}
#endif
