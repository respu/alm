#include <iostream>
#include <sstream>
#include "alm/json.h"

int main()
{
  std::stringstream s;
  s <<            "{" 
    <<            "  \"foo\" : 1,"
    <<            "  \"bar\" : false,"
    <<            "  \"person\" : {\"name\" : \"GWB\", \"age\" : 60},"
    <<            "  \"data\": [\"abcd\", 42, 54.7]"
    <<            "}";

  alm::json_object obj;
  obj.parse(s);

  std::cout << obj.get("foo").getNumber() << std::endl;
  std::cout << obj.get("bar").getBool() << std::endl;
  std::cout << obj.get("person").get("name").getString() << std::endl;
  std::cout << obj.get("data").at(0).getString() << std::endl;
}
