#include "channel.h"

Channel::Channel(){
  enable = false;
  direction = ToRam;
  step = Increment;
  sync = Manual;
  trigger = false;
  chop_dma_sz = 0;
  chop_cpu_sz = 0;
  dummy = 0;

  base = 0;

  block_size = 0;
  block_count = 0;
}

uint32_t Channel::control(){
  uint32_t r = 0;

  r |= (uint32_t)direction << 0;
  r |= (uint32_t)step << 1;
  r |= (uint32_t)chop << 8;
  r |= (uint32_t)sync << 9;
  r |= (uint32_t)chop_dma_sz << 16;
  r |= (uint32_t)chop_cpu_sz << 20;
  r |= (uint32_t)enable << 24;
  r |= (uint32_t)trigger << 28;
  r |= (uint32_t)dummy << 29;

  return r;
}

void Channel::set_control(uint32_t value){
  switch(value & 1 != 0){
    case true:
      direction = FromRam;
      break;
    case false:
      direction = ToRam;
      break;
  }

  switch((value >> 1) & 1 != 0){
    case true:
      step = Decrement;
      break;
    case false:
      step = Increment;
      break;
  }

  chop = (value >> 8) & 1 != 0;

  switch((value >> 9) & 3){
    case 0:
      sync = Manual;
      break;
    case 1:
      sync = Request;
      break;
    case 2:
      sync = LinkedList;
      break;
    default:
      printf("Unknown dma sync mode: %x\n", (value >> 9) & 3);
      exit(1);
  }

  chop_dma_sz = (uint8_t)((value >> 16) & 7);
  chop_cpu_sz = (uint8_t)((value >> 20) & 7);

  enable = (value >> 24) & 1 != 0;
  trigger = (value >> 28) & 1 != 0;
  dummy = (uint8_t)((value >> 29) & 3);
}

// Retrieve the channel's base address
uint32_t Channel::get_base(){
  return base;
}

// Set channel base address. Only bits [0:23] are significant so only 16MB are addressable by the DMA
void Channel::set_base(uint32_t value){
  base = value & 0xffffff;
}


// Retrieve value of the Block Control register
uint32_t Channel::block_control(){
  uint32_t bs = (uint32_t)block_size;
  uint32_t bc = (uint32_t)block_count;

  return (bc << 16) | bs;
}

// Set value of the Block Control register
void Channel::set_block_control(uint32_t value){
  block_size = (uint16_t)value;
  block_count = (uint16_t)(value >> 16);
}
