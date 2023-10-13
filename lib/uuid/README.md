# SnowFlake

Snowflake id generation algorithm implemented by cpp
```cpp
// snowflake algorithm  ==> id generation algorithm
// 64-----------63-------------22----------------12-----------------0
// symbol bit(1) | time bit(41) | machine bit(10) | increas bit(12) |
  
// time bit store the difference between the start timestamp and the current timestamp
// machine bit include 5 bit worker id and 5 bit datacenter id

// example:
#include <iostream>
#include "snowflake.h"

int main(int argc, char *argv[])
{
    for (int i = 0; i < 1000; i++) {
        std::cout << i << " --> " << SnowFlake::instance().nextId() << std::endl;
    }
    return 0;
}
```
