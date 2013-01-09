#ifndef __PARAMETER__
#define __PARAMETER__

#include <chrono>
#include <iostream>

struct parameter
{
  double x; 
  double y; 
};

class parameter_processor
{
public:
  static void next(parameter &param)
  {
    static double yy       = 125.0;

    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    param.x = milliseconds;
    param.y = yy;
  }

};

#endif
