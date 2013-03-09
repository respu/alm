#include <sstream>
#include <algorithm>
#include "alm/json.h"

namespace alm
{

json_value::json_value()
  : m_type(JSON_UNINIT)
{
}

void json_value::deserialize(std::stringstream &input, memory_pool &pool)
{
  assert(m_type == JSON_UNINIT);

  input >> std::ws;
  char c = input.peek();

  switch(c)
  {
    case '{':
      m_type = JSON_OBJECT;
      parseObject(input, pool);
      break;
    case '[':
      m_type = JSON_ARRAY;
      parseArray(input, pool);
      break; 
    case '"':
      m_type = JSON_STRING;
      parseString(input, pool);
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
      output << '"' << m_string->c_str() << '"';
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
}

void json_value::putNull()
{
  assert(m_type == JSON_UNINIT);

  m_type = JSON_NULL;
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

void json_value::parseNull(std::stringstream &input)
{
  json::check(input, "null");
}

void json_value::parseString(std::stringstream &input, memory_pool &pool)
{
  std::size_t length = json::stringLength(input);
  m_string = json::createString(length, pool);
  json::fillString(input, *m_string); 
}

void json_value::parseArray(std::stringstream &input, memory_pool &pool)
{
  m_array = json::createArray(pool);
  m_array->deserialize(input);
}

void json_value::parseObject(std::stringstream &input, memory_pool &pool)
{
  m_object = json::createObject(pool);
  m_object->deserialize(input);
}

json_array::json_array(memory_pool &pool)
  : m_pool(pool), m_values(allocator<json_list_node>(pool))
{
}

json_array::json_array(json_array &&other)
  : m_pool(other.m_pool), m_values(std::move(other.m_values))
{
}

json_array::~json_array()
{
}

void json_array::deserialize(std::stringstream &input)
{
  json::check(input, "[");

  do
  {
    json_value value;
    value.deserialize(input, m_pool);
    m_values.push_back(std::move(value));
  }
  while(json::match(input, ","));

  json::check(input, "]");
}

void json_array::serialize(std::stringstream &output)
{
  output << "[";

  json_list_node* n = m_values.begin();
  while(n)
  {
    n->data.serialize(output);
    if(n->next)
    {
      output << ",";
    }
    n = n->next;
  }
  
  output << "]";
}

json_object::json_object(memory_pool &pool)
  : m_pool(pool), m_values(allocator<json_map_node>(pool))
{
}

json_object::json_object(json_object &&other)
  : m_pool(other.m_pool), m_values(std::move(other.m_values))
{
}

json_object::~json_object()
{
}

void json_object::deserialize(std::stringstream &input)
{
  json::check(input, "{");

  do
  {
    std::size_t length = json::stringLength(input);
    json_string key(length, allocator<char>(m_pool));
    json::fillString(input, key); 

    json::check(input, ":");

    json_value value;
    value.deserialize(input, m_pool);
    m_values.push_back(std::move(json_pair(std::move(key), std::move(value))));
  }
  while(json::match(input, ","));

  json::check(input, "}");
}

void json_object::serialize(std::stringstream &output)
{
  output << "{";

  json_map_node* n = m_values.begin();
  while(n)
  {
    output << '"' << n->data.key.c_str() << '"';
    output << ":";
    n->data.value.serialize(output);

    if(n->next)
    {
      output << ",";
    }
    n = n->next;
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

std::size_t json::stringLength(std::stringstream &input)
{
  json::check(input, "\"");

  char c(0);
  std::size_t begin = input.tellg();

  std::size_t index = begin;
  c = input.peek();
  while(!input.eof() && (c != '"'))
  {
    index++;
    input.seekg(index);
    c = input.peek();
  }
  if(c != '"')
  {
    throw json_exception();
  }
  input.seekg(begin);

  return index - begin;
}

void json::fillString(std::stringstream &input, json_string &s)
{
  input.read(s.c_str(), s.length());
  // Advance the stream index to cover the last quote (") of the string
  input.get();
}

json_string* json::createString(std::size_t length, memory_pool &pool)
{
  json_string* p = (json_string*)pool.alloc(alignSize<json_string>(1));
  :: new (p) json_string(length, allocator<char>(pool));
  return p;
}

json_array* json::createArray(memory_pool &pool)
{
  json_array* p = (json_array*)pool.alloc(alignSize<json_array>(1));
  :: new (p) json_array(pool);
  return p;
}

json_object* json::createObject(memory_pool &pool)
{
  json_object* p = (json_object*)pool.alloc(alignSize<json_object>(1));
  :: new (p) json_object(pool);
  return p;
}

json_document::json_document()
  : m_root(m_pool)
{
}

json_document::json_document(std::size_t size)
  : m_pool(size), m_root(m_pool)
{
}

json_document::~json_document()
{
}

void json_document::deserialize(std::stringstream &input)
{
  m_root.deserialize(input);
}

void json_document::serialize(std::stringstream &output)
{
  m_root.serialize(output);
}

memory_pool& json_document::pool()
{
  return m_pool;
}

json_object& json_document::root()
{
  return m_root;
}

}
