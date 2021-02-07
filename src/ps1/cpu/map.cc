#include "map.h"

namespace map{
  Range::Range(uint32_t start, uint32_t length){
    this->start = start;
    this->length = length;
  }

  // Returns the offset if the address is within range
  uint32_t Range::contains(uint32_t addr) const{
    if(addr >= start && addr < start + length){
      return addr - start;
    }
    else{
      printf("Invalid memory address: %x", addr);
      exit(1);
    }
  }

  uint32_t Range::mask_region(uint32_t addr) const{
    size_t idx = addr >> 29;
    return addr & REGION_MASK[idx];
  }

  const Range *BIOS = new Range(0xbfc00000, 512*1024);
  const Range *MEMCONTROL = new Range(0x1f801000, 36);
  // Register that has something to do with RAM configuration, configured by the BIOS
  const Range *RAM_SIZE = new Range(0x1f801060, 4);

  // Cache control register, full address since it's in KSEG2
  const Range *CACHECONTROL = new Range(0xfffe0130, 4);

  // RAM
  const Range *RAM = new Range(0xa0000000, 2*1024*1024);

  // Unknown registers
  const Range *SYS_CONTROL = new Range(0x1f801000, 36);
}
