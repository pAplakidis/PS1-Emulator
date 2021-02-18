#pragma once

#include "bios.h"
#include "ram.h"
#include "map.h"
#include "dma.h"
#include "gpu.h"

// global interconnect
class Interconnect{
public:
  Bios *bios;
  Ram *ram;
  Dma *dma;
  Gpu *gpu;

  Interconnect(Bios *bios);
  uint32_t load32(uint32_t addr);
  uint16_t load16(uint32_t addr);
  uint8_t load8(uint32_t addr);
  void store32(uint32_t addr, uint32_t val);
  void store16(uint32_t addr, uint16_t val);
  void store8(uint32_t addr, uint8_t val);

private:
  uint32_t dma_reg(uint32_t offset);
  void set_dma_reg(uint32_t offset, uint32_t value);
  void do_dma(enum Port port);
  void do_dma_block(enum Port port);
  void do_dma_linked_list(enum Port port);
};

