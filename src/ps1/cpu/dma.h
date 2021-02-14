# pragma once

#include <stdint.h>

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

public:
  Dma();
  uint32_t get_control();
  void set_control(uint32_t value);
  bool irq();
  uint32_t interrupt();
  void set_interrupt(uint32_t value);
};

