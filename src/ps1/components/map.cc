#include "map.h"

namespace map{
  Range::Range(uint32_t start, uint32_t length){
    this->start = start;
    this->length = length;
  }

  // Returns the offset if the address is within range
  uint32_t* Range::contains(uint32_t addr) const{
    if(addr >= start && addr < start + length){
      uint32_t *res = (uint32_t*)malloc(sizeof(uint32_t));
      *res = addr - start;
      return res;
    }
    else{
      //printf("Invalid memory address: %x\n", addr); // this is for debugging
      return NULL;
    }
  }

  // BIOS
  const Range *BIOS = new Range(0x1fc00000, 512*1024);
  
  // Memory latency and expansion mapping
  const Range *MEMCONTROL = new Range(0x1f801000, 36);

  // Register that has something to do with RAM configuration, configured by the BIOS
  const Range *RAM_SIZE = new Range(0x1f801060, 4);

  // Cache control register, full address since it's in KSEG2
  const Range *CACHECONTROL = new Range(0xfffe0130, 4);

  // RAM
  const Range *RAM = new Range(0x00000000, 8*1024*1024);

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
  const Range *GPU = new Range(0x1f801810, 8);

  // ScratchPad: data cache used as a fast 1KB RAM
  const Range *SCRATCH_PAD = new Range(0x1f800000, 1024);

  // Gamepad and memory card controller
  const Range *PAD_MEMCARD = new Range(0x1f801040, 32);

  // CDROM controller
  const Range *CDROM = new Range(0x1f801800, 0x4);

  const Range *MDEC = new Range(0x1f801820, 8);

  // KSEG0 areas
  //const Range *RAM_KSEG0 = new Range(400000000, 2 * 1024 * 1024);

  uint32_t mask_region(uint32_t addr){
    size_t idx = addr >> 29;
    return addr & REGION_MASK[idx];
  }
}
