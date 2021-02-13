#pragma once

#include "bios.h"
#include "ram.h"
#include "map.h"
#include "dma.h"

// global interconnect
class Interconnect{
public:
  Bios *bios;
  Ram *ram;

  Interconnect(Bios *bios);
  uint32_t load32(uint32_t addr);
  uint16_t load16(uint32_t addr);
  uint8_t load8(uint32_t addr);
  void store32(uint32_t addr, uint32_t val);
  void store16(uint32_t addr, uint16_t val);
  void store8(uint32_t addr, uint8_t val);
};

