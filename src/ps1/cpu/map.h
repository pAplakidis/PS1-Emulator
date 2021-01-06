#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

namespace map{

  class Range{

  public:
    uint32_t start, length;

    Range(uint32_t start, uint32_t length);
    uint32_t contains(uint32_t addr);
  };

  extern Range *BIOS; 
  extern Range *MEMCONTROL;
  extern Range *RAM_SIZE;
}

