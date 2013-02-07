#include <sstream>
#include <algorithm>
#include "alm/json.h"

namespace alm
{

void json_value::deserialize(std::stringstream &input)
{
  input >> std::ws;
  char c = input.peek();

  switch(c)
  {
    case '{':
      m_type = JSON_OBJECT;
      parseObject(input);
      break;
    case '[':
      m_type = JSON_ARRAY;
      parseArray(input);
      break; 
    case '"':
      m_type = JSON_STRING;
      parseString(input);
      break;
    case 't':
    case 'f':
      m_type = JSON_BOOL;
      parseBool(input);
      break;
    case 'n':
      m_type = JSON_NULL;
      parseNull(input);
      break;
    default:
      m_type = JSON_NUMBER;
      parseNumber(input);
      break;
  }
}

void json_value::serialize(std::stringstream &output)
{
  switch(m_type)
  {
    case JSON_OBJECT:
      m_object->serialize(output);
      break;
    case JSON_ARRAY:
      m_array->serialize(output);
      break; 
    case JSON_STRING:
      output << '"' << *m_string << '"';
      break;
    case JSON_BOOL:
    {
      std::string s = m_bool ? "true" : "false";
      output << s;
      break;
    }
    case JSON_NULL:
      output << "null";
      break;
    case JSON_NUMBER:
      output << m_number;
      break;
    default:
      break;
  }
}

json_value::~json_value()
{
  switch(m_type)
  {
    case JSON_OBJECT:
      delete m_object;
      break;
    case JSON_ARRAY:
      delete m_array;
      break;
    case JSON_STRING:
      delete m_string;
      break;
    default:
      break;
  }
}

void json_value::parseBool(std::stringstream &input)
{
  if(json::match(input, "true"))
  {
    m_bool = true;
  }
  else if(json::match(input, "false"))
  {
    m_bool = false;
  }
  else
  {
    throw json_exception();
  }
}

void json_value::parseNumber(std::stringstream &input)
{
  input >> std::ws;
  input >> m_number;
  if (input.fail())
  {
    throw json_exception();
  }
}

void json_value::parseString(std::stringstream &input)
{
  m_string = new std::string();
  json::read(input, *m_string);
}

void json_value::parseNull(std::stringstream &input)
{
  json::check(input, "null");
}

void json_value::parseArray(std::stringstream &input)
{
  m_array = new json_array();
  m_array->deserialize(input);
}

void json_value::parseObject(std::stringstream &input)
{
  m_object = new json_object();
  m_object->deserialize(input);
}

json_array::json_array()
{
  m_values = new std::vector<json_value*>();
}

json_array::json_array(json_array &&other)
{
  m_values = other.m_values;
  other.m_values = 0;
}

json_array::~json_array()
{
  if(m_values != 0)
  {
    std::vector<json_value*>::iterator it = m_values->begin();
    for(; it != m_values->end(); ++it)
    {
      delete *it;
    }
    delete m_values;
  }
}

void json_array::deserialize(std::stringstream &input)
{
  json::check(input, "[");

  do
  {
    json_value* value = new json_value();
    value->deserialize(input);
    m_values->push_back(value);
  }
  while(json::match(input, ","));

  json::check(input, "]");
}

void json_array::serialize(std::stringstream &output)
{
  output << "[";

  std::vector<json_value*>::iterator it = m_values->begin();

  for(; it != m_values->end();)
  {
    (*it)->serialize(output);
    ++it;
    if(it != m_values->end())
    {
      output << ",";
    }
  }
  
  output << "]";
}

json_object::json_object()
{
  m_values = new std::map<std::string, json_value*>();
}

json_object::json_object(json_object &&other)
{
  m_values = other.m_values;
  other.m_values = 0;
}

json_object::~json_object()
{
  if(m_values != 0)
  {
    std::map<std::string, json_value*>::iterator it = m_values->begin();
    for(; it != m_values->end(); ++it)
    {
      delete it->second;
    }
    delete m_values;
  }
}

void json_object::deserialize(std::stringstream &input)
{
  json::check(input, "{");

  do
  {
    std::string key;
    json::read(input, key);

    json::check(input, ":");

    json_value* value = new json_value();
    value->deserialize(input);
    m_values->insert(std::pair<std::string,json_value*>(key, value));
  }
  while(json::match(input, ","));

  json::check(input, "}");
}

void json_object::serialize(std::stringstream &output)
{
  output << "{";

  std::map<std::string, json_value*>::iterator it = m_values->begin();
  for(; it != m_values->end();)
  {
    output << it->first;
    output << ":";
    it->second->serialize(output);

    ++it;

    if(it != m_values->end())
    {
      output << ",";
    }
  }

  output << "}";
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

}
