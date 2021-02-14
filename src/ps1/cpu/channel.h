#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// DMA transfer direction
enum Direction{
  ToRam = 0,
  FromRam = 1
};

// DMA transfer step
enum Step{
  Increment = 0,
  Decrement = 1
};

// DMA transfer synchronization mode
enum Sync{
  // Transfer starts when the CPU write to the Trigger bit and transfers everything at once
  Manual = 0,
  // Sync blocks to DMA requests
  Request = 1,
  // Used to transfer GPU command lists
  LinkedList = 2
};

// Per-channel data
class Channel{
private:
  bool enable;
  enum Direction direction;
  enum Step step;
  enum Sync sync;
  // Used to start the DMA transfer when 'sync' is 'Manual'
  bool trigger;
  // If true teh DMA "chops" the transfer and lets the CPU run in the gaps
  bool chop;
  // Chopping DMA window size (log2 number of words)
  uint8_t chop_dma_sz;
  // Chopping CPU window size (log2 number of cycles)
  uint8_t chop_cpu_sz;
  // Unknown 2 RW bits in configuration register
  uint8_t dummy;

  //  DMA start address
  uint32_t base;

public:
  Channel();
  uint32_t control();
  void set_control(uint32_t value);
  uint32_t get_base();
  void set_base(uint32_t value);
};

