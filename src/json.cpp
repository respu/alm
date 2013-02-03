#include <sstream>
#include <algorithm>
#include "alm/json.h"

#include <iostream>

namespace alm
{

void json_number::parse(std::stringstream &input)
{
  input >> std::ws;
  input >> m_data;
  if (input.fail())
  {
    throw json_exception();
  }
}

void json_string::parse(std::stringstream &input)
{
  json::read(input, m_data);
}

void json_bool::parse(std::stringstream &input)
{
  if(json::match(input, "true"))
  {
    m_data = true;
  }
  else if(json::match(input, "false"))
  {
    m_data = false;
  }
  else
  {
    throw json_exception();
  }
}

void json_null::parse(std::stringstream &input)
{
  json::check(input, "null");
}

json_array::~json_array()
{
  std::vector<json_value*>::iterator it = m_values.begin();
  for(; it != m_values.end(); ++it)
  {
    delete *it;
  }
}

void json_array::parse(std::stringstream &input)
{
  json::check(input, "[");

  do
  {
    json_value* value = json::parse(input);
    m_values.push_back(value);
  }
  while(json::match(input, ","));

  json::check(input, "]");
}

json_object::~json_object()
{
  std::map<std::string, json_value*>::iterator it = m_values.begin();
  for(; it != m_values.end(); ++it)
  {
    delete it->second;
  }
}

void json_object::parse(std::stringstream &input)
{
  json::check(input, "{");

  do
  {
    std::string key;
    json::read(input, key);

    json::check(input, ":");

    json_value* value = json::parse(input);

    m_values[key] = value;
  }
  while(json::match(input, ","));

  json::check(input, "}");
}

bool json::match(std::stringstream &input, const char* pattern)
{
  input >> std::ws;
  char c(0);
  const char* index = pattern;
  while(*index != 0 && !input.eof())
  {
    input.get(c);
    if (c != *index)
    {
      input.putback(c);
      while (index > pattern)
      {
        index--;
        input.putback(*index);
      }
      return false;
    }
    index++;
  }
  return *index == 0;
}

void json::check(std::stringstream &input, const char* pattern)
{
  if(!json::match(input, pattern))
  {
    throw json_exception();
  }
}

void json::read(std::stringstream &input, std::string &str)
{
  json::check(input, "\"");

  char c(0);
  input.get(c);
  while(!input.eof() && (c != '"'))
  {
    str.push_back(c);
    input.get(c);
  }
  if(c != '"')
  {
    throw json_exception();
  }
}

json_value* json::parse(std::stringstream &input)
{
  json_value* result = 0;

  input >> std::ws;
  char c = input.peek();

  switch(c)
  {
    case '{':
      result = new json_object();
      break;
    case '[':
      result = new json_array();
      break; 
    case '"':
      result = new json_string();
      break;
    case 't':
    case 'f':
      result = new json_bool();
      break;
    case 'n':
      result = new json_null();
      break;
    default:
      result = new json_number();
      break;
  }
  result->parse(input);

  return result;
}

json_holder::json_holder()
  : m_value(0)
{
}

void json_holder::operator<<(std::stringstream &input)
{
  if(m_value != 0)
  {
    delete m_value;
  }
  m_value = json::parse(input);
}

json_holder::~json_holder()
{
  delete m_value;
}

}
