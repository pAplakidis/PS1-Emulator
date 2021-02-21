#include "map.h"

namespace map{
  Range::Range(uint32_t start, uint32_t length){
    this->start = start;
    this->length = length;
  }

  // TODO: DEBUG THIS (check if it's right)
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

  // TODO: maybe move these to the interconnect file?
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

  // SPU registers
  const Range *SPU = new Range(0x1f801c00, 640);

  // Expansion region 2
  const Range *EXPANSION_2 = new Range(0x1f802000, 66);

  // Expansion region 1
  const Range *EXPANSION_1 = new Range(0x1f000000, 8 * 1024 * 1024);

  // Interrupt Control registers (status and mask)
  const Range *IRQ_CONTROL = new Range(0x1f801070, 8);

  // Timer registers (independent of timers on the Playstation, we just ignore these)
  // TODO: check if this range is correct
  const Range *TIMERS= new Range(0x1f801100, 48);

  // Direct Memory Access registers (used to move data between peripherals and RAM withought involving the CPU)
  const Range *DMA = new Range(0x1f801080, 0x80);
  
  // GPU Registers (TODO: check if this range is correct)
  const Range *GPU= new Range(0x1f801810, 8);

  uint32_t mask_region(uint32_t addr){
    size_t idx = addr >> 29;
    return addr & REGION_MASK[idx];
  }
}
