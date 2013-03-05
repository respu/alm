#include <iostream>
#include <sstream>
#include <fstream>
#include "alm/json.h"
#include "alm/memory.h"

void test_huge()
{
  std::cout << "start read file" << std::endl;

  std::string line;
  std::stringstream ss;
  std::ifstream myfile ("citylots.json");
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
      getline (myfile,line);
      ss << line;
    }
    myfile.close();
  }
  std::cout << "end read file" << std::endl;

  std::cout << "start deserialization" << std::endl;

  alm::json_document doc;
  doc.deserialize(ss);

  std::cout << "end deserialization" << std::endl;

  std::cout << "start serialization" << std::endl;

  std::stringstream output;
  doc.serialize(output);

  std::cout << "end serialization" << std::endl;

}
/*
void test_put()
{
  std::stringstream ss;

  alm::json_object obj;
  obj.put<std::string>("name","pepe");
  obj.putNull("c1");

  alm::json_array a;
  a.put<double>(1);
  a.putNull();
  obj.put<alm::json_array>("lista",std::move(a));

  obj.serialize(ss);
  std::cout << ss.str() << std::endl;

  alm::json_object o;
  o.deserialize(ss);

  std::stringstream ss2;
  o.serialize(ss2);

  std::cout << ss2.str() << std::endl;

  std::cout << o.has<std::string>("name") << std::endl;
  std::cout << o.has<std::string>("c1") << std::endl;
}

void test()
{
  std::stringstream s;
  s <<            "{" 
    <<            "  \"foo\" : 1,"
    <<            "  \"bar\" : false,"
    <<            "  \"person\" : {\"name\" : \"GWB\", \"age\" : 60},"
    <<            "  \"data\": [\"abcd\", 42, 54.7],"
    <<            "  \"foo2\" : 1,"
    <<            "  \"bar2\" : false,"
    <<            "  \"person2\" : {\"name\" : \"GWB\", \"age\" : 60},"
    <<            "  \"data2\": [\"abcd\", 42, 54.7]"
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
void test_pool()
{
  alm::memory_pool pool(10240000); 
  for(int i = 0; i<20000000; ++i)
  {
    alm::json_value* v = pool.create<alm::json_value>(2);
    v->putNull();
  }


  for(int i = 0; i<20000000; ++i)
  {
    alm::json_value* v = pool.create<alm::json_value>(1);
    v->putNull();
  }

}
*/
void test_memory()
{
  alm::memory_pool pool;
  alm::allocator<int> alloc(pool);

  std::vector<int, alm::allocator<int>> v1(alloc);
  v1.push_back(1);

  std::vector<int, alm::allocator<int>> v2(alloc);
  v2.push_back(1);

  pool.clear();
}

int main()
{
  test_huge();

  return 0;
}
