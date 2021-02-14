# pragma once

#include <stdint.h>

class Dma{
private:
  // DMA control register
  uint32_t control;

public:
  Dma();
  uint32_t get_control();
  void set_control(uint32_t value);
};

