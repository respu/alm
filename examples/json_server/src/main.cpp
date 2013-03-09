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

  ss.seekp(0, std::ios::end);
  std::stringstream::pos_type offset = ss.tellp();
  std::cout << offset << std::endl;
}

void test_put()
{
  std::stringstream ss;

  alm::json_document doc;

  alm::json_object& obj = doc.root();
  
  alm::json_string pepe("pepe", alm::allocator<char>(doc.pool()));

  obj.put<alm::json_string>("name", std::move(pepe));
  obj.putNull("c1");

  alm::json_array a(doc.pool());
  a.put<double>(1.0);
  a.putNull();
  obj.put<alm::json_array>("lista",std::move(a));

  doc.serialize(ss);
  std::cout << ss.str() << std::endl;
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


  alm::json_document doc;
  doc.deserialize(s);
  alm::json_object& obj = doc.root();

  std::cout << obj.get<double>("foo") << std::endl;
  std::cout << obj.get<bool>("bar") << std::endl;
  std::cout << obj.get<alm::json_object>("person").get<alm::json_string>("name").c_str() << std::endl;
  std::cout << obj.get<alm::json_array>("data").get<alm::json_string>(0).c_str() << std::endl;


  std::stringstream ss;
  doc.serialize(ss);

  std::cout << ss.str() << std::endl;
}

/*
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
  test();

  return 0;
}
