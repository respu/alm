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

  alm::json_holder obj;
  obj << s;

}
