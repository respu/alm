#include <iostream>
#include <sstream>
#include "alm/json.h"

int main()
{
  {
  std::stringstream ss;

  alm::json_object obj;
  std::string s("pepe");
  obj.put<std::string>("name",std::move(s));

  alm::json_array a;
  a.put<double>(1);
  obj.put<alm::json_array>("lista",std::move(a));

  obj.serialize(ss);
  std::cout << ss.str() << std::endl;
  }

  return 0;

  {
  std::stringstream s;
  s <<            "{" 
    <<            "  \"foo\" : 1,"
    <<            "  \"bar\" : false,"
    <<            "  \"person\" : {\"name\" : \"GWB\", \"age\" : 60},"
    <<            "  \"data\": [\"abcd\", 42, 54.7]"
    <<            "}";

  alm::json_object obj;
  obj.deserialize(s);

  std::cout << obj.get<double>("foo") << std::endl;
  std::cout << obj.get<bool>("bar") << std::endl;
  std::cout << obj.get<alm::json_object>("person").get<std::string>("name") << std::endl;
  std::cout << obj.get<alm::json_array>("data").get<std::string>(0) << std::endl;


  std::stringstream ss;
  obj.serialize(ss);

  std::cout << ss.str() << std::endl;
  }
  
}
