#ifndef __ALM__json___
#define __ALM__json___

#include <vector>
#include <map>
#include <sstream>
#include <exception>

namespace alm
{

/************************************************
 * Use tagged union
 *
 * Encapsule everything as much as possible
 * (Value.serialize, Value.deserialize, ...)
 *
 * Use swith statements instead of if/else where
 * possible for the shake of clarity
 ************************************************/


struct json_exception : public std::exception {};

class json_value
{
public:
  json_value(){}

  virtual ~json_value(){}

  virtual void parse(std::stringstream &input)      = 0;
  virtual void deserialize(std::stringstream &input)= 0;

  virtual double       getNumber()                  = 0;
  virtual std::string& getString()                  = 0;
  virtual bool         getBool()                    = 0;
  virtual json_value&  at(unsigned int index)       = 0;
  virtual json_value&  get(const char* key)         = 0;
  virtual unsigned int size()                       = 0;
};

class json_number : public json_value 
{
public:
  json_number(){}

  virtual ~json_number(){}

  virtual void parse(std::stringstream &input);

  virtual double       getNumber()                 { return m_data;          }
  virtual std::string& getString()                 { throw json_exception(); }
  virtual bool         getBool()                   { throw json_exception(); }
  virtual json_value&  at(unsigned int index)      { throw json_exception(); }
  virtual json_value&  get(const char* key)        { throw json_exception(); }
  virtual unsigned int size()                      { throw json_exception(); } 

private:
  double m_data;
};

class json_string : public json_value
{
public:
  json_string(){}

  virtual ~json_string(){}

  virtual void parse(std::stringstream &input);

  virtual double       getNumber()                 { throw json_exception(); }
  virtual std::string& getString()                 { return m_data;          }
  virtual bool         getBool()                   { throw json_exception(); }
  virtual json_value&  at(unsigned int index)      { throw json_exception(); }
  virtual json_value&  get(const char* key)        { throw json_exception(); }
  virtual unsigned int size()                      { throw json_exception(); } 

private:
  std::string m_data;
};

class json_bool : public json_value
{
public:
  json_bool(){}

  virtual ~json_bool(){}

  virtual void parse(std::stringstream &input);

  virtual double       getNumber()                 { throw json_exception(); }
  virtual std::string& getString()                 { throw json_exception(); }
  virtual bool         getBool()                   { return m_data;          }
  virtual json_value&  at(unsigned int index)      { throw json_exception(); }
  virtual json_value&  get(const char* key)        { throw json_exception(); }
  virtual unsigned int size()                      { throw json_exception(); } 

private:
  bool m_data;
};

class json_null : public json_value
{
public:
  json_null(){}

  virtual ~json_null(){}

  virtual void parse(std::stringstream &input);

  virtual double       getNumber()                 { throw json_exception(); }
  virtual std::string& getString()                 { throw json_exception(); }
  virtual bool         getBool()                   { throw json_exception(); }
  virtual json_value&  at(unsigned int index)      { throw json_exception(); }
  virtual json_value&  get(const char* key)        { throw json_exception(); }
  virtual unsigned int size()                      { throw json_exception(); } 

};

class json_array : public json_value
{
public:
  json_array(){}

  virtual ~json_array();

  virtual void parse(std::stringstream &input);

  virtual double       getNumber()                 { throw json_exception();     }
  virtual std::string& getString()                 { throw json_exception();     }
  virtual bool         getBool()                   { throw json_exception();     }
  virtual json_value&  at(unsigned int index)      { return *(m_data.at(index)); }
  virtual json_value&  get(const char* key)        { throw json_exception();     }
  virtual unsigned int size()                      { return m_data.size();       } 

private:
  std::vector<json_value*> m_data;
};

class json_object : public json_value
{
public:
  json_object(){}

  virtual ~json_object();

  virtual void parse(std::stringstream &input);

  virtual double       getNumber()                 { throw json_exception(); }
  virtual std::string& getString()                 { throw json_exception(); }
  virtual bool         getBool()                   { throw json_exception(); }
  virtual json_value&  at(unsigned int index)      { throw json_exception(); }
  virtual json_value&  get(const char* key)        { return *(m_data[key]);  }
  virtual unsigned int size()                      { throw json_exception(); } 

private:
  std::map<std::string, json_value*> m_data;
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
