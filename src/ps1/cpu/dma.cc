#include "dma.h"

Dma::Dma(){
  // reset value taken from the Nocash PSX spec
  control = 0x07654321;
}

// Retrieve the value of the control register
uint32_t Dma::get_control(){
  return control;
}

// Set the value of the control register
void Dma::set_control(uint32_t value){
  control = value;
}

// Return the status of the DMA interrupt
bool Dma::irq(){
  uint8_t channel_irq = channel_irq_flags & channel_irq_en;
  return force_irq || (irq_en && channel_irq != 0);
}

// Retrieve the value of the interrupt register
uint32_t Dma::interrupt(){
  uint32_t r = 0;

  r |= (uint32_t)irq_dummy;
  r |= (uint32_t)force_irq << 15;
  r |= (uint32_t)channel_irq_en << 16;
  r |= (uint32_t)irq_en << 23;
  r |= (uint32_t)channel_irq_flags << 24;
  r |= (uint32_t)irq() << 31;

  return r;
}

// Set the value of the interrupt register
void Dma::set_interrupt(uint32_t value){
  // Unknown what bits [5:0] do
  irq_dummy = (uint8_t)(value & 0x3f);

  force_irq = (value >> 15) & 1 != 0;
  channel_irq_en = (uint8_t)((value >> 16) & 0x7f);
  irq_en = (value >> 23) & 1 != 0;

  // Writing 1 to a flag resets it
  uint8_t ack = (uint8_t)((value >> 24) & 0x3f);
  channel_irq_flags &= !ack;
}

