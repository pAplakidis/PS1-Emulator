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

enum Direction Channel::get_direction(){
  return direction;
}

enum Step Channel::get_step(){
  return step;
}

enum Sync Channel::get_sync(){
  return sync;
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

// Return true if the channel has been started
bool Channel::active(){
  // In manual sync mode tha CPU must set the "trigger" bit to start the tranfser
  bool trigger;
  switch(sync){
    case Manual:
      trigger = this->trigger;
      break;
    default:
      return true;
  }
  return enable && trigger;
}

// Return the DMA transfer size in bytes or None for linked list mode
uint32_t* Channel::transfer_size(){
  uint32_t bs = (uint32_t)block_size;
  uint32_t bc = (uint32_t)block_count;

  uint32_t *ret = (uint32_t*)malloc(sizeof(uint32_t));

  switch(sync){
    // For manual mode only the block size is used
    case Manual:
      *ret = bs;
      break;
    // In DMA request mode we must transfer "bc" blocks
    case Request:
      *ret = bc * bs;
      break;
    // In linked list mode the size is not known ahead of time: we stop when we encounter the "end of list" marker (0xffffff)
    case LinkedList:
      return NULL;
  }

  return ret;
}

// Set the channel status to "completed" state
void Channel::done(){
  enable = false;
  trigger = false;

  // TODO: Need to set the correct value for the other fields (in particular interrupts)
}

