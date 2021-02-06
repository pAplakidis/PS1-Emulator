#include "map.h"

namespace map{
  Range::Range(uint32_t start, uint32_t length){
    this->start = start;
    this->length = length;
  }

  // Returns the offset if the address is within range
  uint32_t Range::contains(uint32_t addr){
    if(addr >= start && addr < start + length){
      return addr - start;
    }
    else{
      printf("Invalid memory address: %x", addr);
      exit(1);
    }
  }

  Range *BIOS = new Range(0xbfc00000, 512*1024);
  Range *MEMCONTROL = new Range(0x1f801000, 36);
  Range *RAM_SIZE = new Range(0x1f801060, 4);

  // Cache control register
  Range *CACHECONTROL = new Range(0xfffe0130, 4);

  // RAM
  Range *RAM = new Range(0xa0000000, 2*1024*1024);
}
