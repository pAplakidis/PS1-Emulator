# pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "channel.h"

// The 7 DMA ports
enum Port{
  // Macroblock decoder input
  MdecIn = 0,
  // Macroblock decoder output
  MdecOut = 1,
  // Graphics Processing Unit
  GPU = 2,
  // CC-ROM drive
  CdRom = 3,
  // Sound Processing Unit
  Spu = 4,
  // Extension port
  Pio = 5,
  // Used to clear the ordering table
  Otc = 6
};

namespace PORT{
  extern enum Port from_index(uint32_t index);
}

class Dma{
private:
  // DMA control register
  uint32_t control;

  // master IRQ enable
  bool irq_en;

  // IRQ enable for individual channels
  uint8_t channel_irq_en;

  // RIQ flags for individual channels
  uint8_t channel_irq_flags;

  // When set the interrupt is active unconditionally (even if 'irq_en' is false)
  bool force_irq;

  // Store bits [0:5] of the interrupt registers (RW) and send them back untouched on reads
  uint8_t irq_dummy;

  // The 7 channel instaces
  Channel *channels[7];

public:
  Dma();
  uint32_t get_control();
  void set_control(uint32_t value);
  bool irq();
  uint32_t interrupt();
  void set_interrupt(uint32_t value);
  Channel* channel(enum Port port); // TODO: check if we need to implement the mutable version (all pointers are read/write in C++ so probably we don't)
  enum Port from_index(uint32_t index);
};

