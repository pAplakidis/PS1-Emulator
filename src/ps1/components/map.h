#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// This is PS1's memory map
namespace map{

  class Range{

  public:
    uint32_t start, length;

    Range(uint32_t start, uint32_t length);
    uint32_t *contains(uint32_t addr) const;
  };

  extern const Range *BIOS;
  extern const Range *MEMCONTROL;
  extern const Range *RAM_SIZE;
  extern const Range *CACHECONTROL;
  extern const Range *RAM;
  extern const Range *SPU;
  extern const Range *EXPANSION_2;
  extern const Range *EXPANSION_1;
  extern const Range *IRQ_CONTROL;
  extern const Range *TIMERS;
  extern const Range *DMA;
  extern const Range *GPU;

  // Mask array used to strip the region bits of the adrress
  // The mask is selected using the 3 MSBs of the address so each entry effectively matches 512KB of the address space
  // KSEG2 is not touched since it doesn't share anything with other regions

  // Mask array used to strip the region bits of the adrress
  // The mask is selected using the 3 MSBs of the address so each entry effectively matches 512KB of the address space
  // KSEG2 is not touched since it doesn't share anything with other regions
  const uint32_t REGION_MASK[8] = {
                        // KUSEG: 2048MB
                        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                        // KSEG0: 512MB
                        0x7fffffff,
                        // KSEG1: 512MB
                        0x1fffffff,
                        // KSEG2: 1024MB
                        0xffffffff, 0xffffffff
                        };

  uint32_t mask_region(uint32_t addr);
}

