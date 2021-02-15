#include "interconnect.h"

Interconnect::Interconnect(Bios *bios){
  // TODO: delete this object from memory after done booting game
  this->bios = bios;
  ram = new Ram();

  // DMA registers
  dma = new Dma();
}

// load a 32bit word at addr
uint32_t Interconnect::load32(uint32_t addr){
  // Check for address alignment (must be an address-multiple of 32bits)
  if(addr % 4 != 0){
    printf("Unaligned load32 address: %8x\n", addr);
    exit(1);
  }
  
  uint32_t abs_addr = map::mask_region(addr);

  if(uint32_t offset = map::BIOS->contains(addr)){
    return bios->load32(offset);
  }
  else{
    printf("Cannot fetch address at %x\n", addr);
    exit(1);
  }

  if(uint32_t offset = map::IRQ_CONTROL->contains(abs_addr)){
    printf("IRQ control read %x\n", offset);
  }

  if(uint32_t offset = map::DMA->contains(abs_addr)){
    printf("DMA read %08x\n", abs_addr);
    return 0;
  }

  if(uint32_t offset = map::GPU->contains(abs_addr)){
    printf("GPU read %x\n", offset);
    switch(offset){
      // GPUSTAT: set bit 28 to signal that the GPU is ready to receive DMA blocks
      case 4:
        return 0x10000000;
      default:
        return 0;
    }
  }

  if(uint32_t offset = map::DMA->contains(abs_addr)){
    return dma_reg(offset);
  }

  if(uint32_t offset = map::GPU->contains(abs_addr)){
    switch(offset){
      // GPUSTAT: set bit 26, 27, 28 tosignal that the GPU is ready for DMA and CPU access.
      // This way the BIOS won't dead lock waiting for an event that will never come.
      case 4:
        return 0x1c000000;
      default:
        return 0;
    }
  }
}

// Load 16bit halfword at 'addr'
uint16_t Interconnect::load16(uint32_t addr){
  uint32_t abs_addr = map::mask_region(addr);

  if(uint32_t offset = map::SPU->contains(abs_addr)){
    printf("Unhandled read from SPU register %08x\n", abs_addr);
    return 0;
  }

  if(uint32_t offset = map::RAM->contains(abs_addr)){
    return ram->load16(offset);
  }

  if(uint32_t offset = map::IRQ_CONTROL->contains(abs_addr)){
    printf("IRQ control read %x\n", offset);
    return 0;
  }

  printf("Unhandled load16 at address %08x\n", addr);
  exit(1);
}

// load byte at 'addr'
uint8_t Interconnect::load8(uint32_t addr){
  uint32_t abs_addr = map::mask_region(addr);
  
  if(uint32_t offset = map::RAM->contains(abs_addr)){
    return ram->load8(offset);
  }

  if(uint32_t offset = map::BIOS->contains(abs_addr)){
    return bios->load8(offset);
  }

  if(uint32_t _ = map::EXPANSION_1->contains(abs_addr)){
    // no expansion implemented
    return 0xff;
  }

  printf("Unhandled load8 at address %8x\n", addr);
  exit(1);
}

// TODO: the only peripheral we support right now is BIOS ROM and we can't write to it, come back and complete this later
// store 32bit word val into addr
void Interconnect::store32(uint32_t addr, uint32_t val){
  // Check for address alignment (must be an address-multiple of 32bits)
  if(addr % 4 != 0){
    printf("Unaligned store32 address: %x\n", addr);
    exit(1);
  }

  uint32_t abs_addr = map::mask_region(addr);

  // Handle Expansion mapping
  if(uint32_t offset = map::MEMCONTROL->contains(addr)){
    switch(offset){
      // Expansion 1 base address
      case 0:
        if(val != 0xf1000000){
          printf("Bad Expansion 1 base address: %x\n", val);
          exit(1);
        }
      case 4:
        if(val != 0xf1802000){
          printf("Bad Expansion 2 base address: %x\n", val);
          exit(1);
      }
      default:
        printf("Unhandled write to MEMCONTROL register\n");
    }
    return;
  }

  if(uint32_t offset = map::IRQ_CONTROL->contains(abs_addr)){
    printf("IRQ_control: %x <- %08x\n", offset, val);
    return;
  }

  if(uint32_t offset = map::DMA->contains(abs_addr)){
    printf("DMA write: %08x: %08x\n", abs_addr, val);
    return;
  }

  if(uint32_t offset = map::GPU->contains(abs_addr)){
    printf("GPU write %x: %08x\n", offset, val);
    return;
  }

  if(uint32_t offset = map::TIMERS->contains(abs_addr)){
    printf("Unhandled write to timer register %x: %08x\n", offset, val);
    return;
  }

  if(uint32_t offset = map::DMA->contains(abs_addr)){
    return set_dma_reg(offset, val);
  }
}

// TODO: the only peripheral we support right now is BIOS ROM and we can't write to it, come back and complete this later
// Store 16bit halfword 'val' into 'addr'
void Interconnect::store16(uint32_t addr, uint16_t val){
  uint32_t abs_addr = map::mask_region(addr);

  if(uint32_t offset = map::RAM->contains(abs_addr)){
    return ram->store16(offset, val);
  }

  if(uint32_t offset = map::SPU->contains(abs_addr)){
    printf("Unhandled write to SPU register %x\n", offset);
    return;
  }
  
  if(uint32_t offset = map::TIMERS->contains(abs_addr)){
    printf("Unhandled write to timer register %x\n",  offset);
    return;
  }

  if(uint32_t offset = map::IRQ_CONTROL->contains(abs_addr)){
    printf("IRQ control write %x: %04x\n", offset, val);
    return;
  }
}

// TODO: the only peripheral we support right now is BIOS ROM and we can't write to it, come back and complete this later
// Store 8bit halfword 'val' into 'addr'
void Interconnect::store8(uint32_t addr, uint8_t val){
  uint32_t abs_addr = map::mask_region(addr);

  if(uint32_t offset = map::EXPANSION_2->contains(abs_addr)){
    printf("Unhandled write to SPU register %x\n", offset);
    return ram->store8(offset, val);
  }
  
  printf("Unhandled store8 into address %8x\n", addr);
}

// DMA register read
uint32_t Interconnect::dma_reg(uint32_t offset){
  uint32_t major = (offset & 0x70) >> 4;
  uint32_t minor = offset & 0xf;

  /*
  switch(offset){
    case 0x70:
      return dma->get_control();
    default:
      printf("Unhandled DMA access: %08x\n", offset);
      exit(1);
  }
  */

  switch(major){
    // Per-channel registers
    case 0 ... 6:
    {
      Channel *channel = dma->channel(PORT::from_index(major));
      switch(minor){
        case 0:
          return channel->get_base();
        case 4:
          return channel->block_control();
        case 8:
          return channel->control();
        default:
          printf("Unhandled DMA read at %x\n", offset);
          exit(1);
      }
      break;
    }
    // Common DMA registers
    case 7:
    {
      switch(minor){
        case 0:
          return dma->get_control();
        case 4:
          return dma->interrupt();
        default:
          printf("Unhandled DMA read at %x\n", offset);
          exit(1);
      }
      break;
    }
    default:
    {
      printf("Unhandled DMA read at %x\n", offset);
      exit(1);
    }
  }
}

// DMA register write
void Interconnect::set_dma_reg(uint32_t offset, uint32_t value){
  uint32_t major = (offset & 0x70) >> 4;
  uint32_t minor = offset & 0xf;

  /*
  switch(offset){
    case 0x70:
      dma->set_control(value);
      break;
    default:
      printf("Unhandled DMA write access: %08x <- %08x\n", offset, value);
      exit(1);
  }
  */

  enum Port *active_port = (enum Port*)malloc(sizeof(enum Port));

  switch(major){
    // Per-channel registers
    case 0 ... 6:
    {
      enum Port port = PORT::from_index(major);
      Channel *channel = dma->channel(port);

      switch(minor){
        case 0:
          channel->set_base(value);
          break;
        case 4:
          channel->set_block_control(value);
          break;
        case 8:
          channel->set_control(value);
          break;
        default:
          printf("Unhandled DMA write at %x <- %08x\n", offset, value);
          exit(1);
      }

      if(channel->active()){
        *active_port = port;
      }else{
        active_port = NULL;
      }
      break;
    }
    // Common DMA registers
    case 7:
    {
      switch(minor){
        case 0:
          dma->set_control(value);
          break;
        case 4:
          dma->set_interrupt(value);
          break;
        default:
          printf("Unhandled DMA write at %x <- %08x\n", offset, value);
          exit(1);
      }

      active_port = NULL;
      break;
    }
    default:
      printf("Unhandled DMA write at %x <- %08x\n", offset, value);
      exit(1);
  }
 if(active_port != NULL){
    do_dma(*active_port);
 }
}

// Execute DMA transfer for a port
void Interconnect::do_dma(enum Port port){
  // DMA transfer has been started, for now we process everything in one pass (i.e. no chopping or priority handling)
  switch(dma->channel(port)->get_sync()){
    case LinkedList:
      do_dma_linked_list(port);
      break;
    default:
      do_dma_block(port);
      break;
  }
}

// DMA block copy 
void Interconnect::do_dma_block(enum Port port){
  Channel *channel = dma->channel(port);

  int32_t increment;
  switch(channel->get_step()){
    case Increment:
      increment = 4;
      break;
    case Decrement:
      increment = -4;
      break;
  }

  uint32_t addr = channel->get_base();

  // Transfer size in words
  uint32_t remsz;
  if(uint32_t *temp = channel->transfer_size()){
    remsz = *temp;
  }else{
    printf("Couldn't figure out DMA block transfer size\n");
    exit(1);
  }

  while(remsz > 0){
    // Just mask addr (i.e. the RAM address wraps and the two LSB are ignored)
    uint32_t cur_addr = addr & 0x1ffffc;

    switch(channel->get_direction()){
      case FromRam:
        printf("Unhandled DMA direction\n");
        exit(1);
      case ToRam:
      {
        uint32_t src_word;
        switch(port){
          case Otc:
            switch(remsz){
              // Last entry contains the end of table marker
              case 1:
                src_word = 0xffffff;
                break;
              // Pointer to the previous entry
              default:
                src_word = (addr - 4) & 0x1fffff;
                break;
            }
            break;
          default:
            printf("Unhandled DMA source port %x\n", (uint8_t)port);
            exit(1);
        }
        ram->store32(cur_addr, src_word);
      }
      break;
    }
    addr = addr + increment;
    remsz--;
  }
  channel->done();
}

// Emulate DMA transfer for linked list synchronization mode
void Interconnect::do_dma_linked_list(enum Port port){
  Channel *channel = dma->channel(port);
  uint32_t addr = channel->get_base() & 0x1ffffc;

  if(channel->get_direction() == ToRam){
    printf("Invalid DMA direction for linked list mode\n");
    exit(1);
  }

  // DMA doesn't support linked list for anything appart the GPU
  if(port != Gpu){
    printf("Attempted linked list DMA on port %x\n", (uint8_t)port);
    exit(1);
  }

  while(1){
    // In linked list mode, each entry starts with a "header" word. The high byte contains the number of words in the "packet" (not counting the header word)
    uint32_t header = ram->load32(addr);
    uint32_t remsz = header >> 24;
    
    while(remsz > 0){
      addr = (addr + 4) & 0x1ffffc;
      uint32_t command = ram->load32(addr);
      printf("GPU command %08x\n", command);
      remsz--;
    }
    // The end-of-table marker is usually 0xffffff but mednafen only checks for the MSB so maybethat's what the harware does. Since this bit is not part of any valid address it makes some sense. TODO: test that
    if(header & 0x800000 != 0){
      break;
    }

    addr = header & 0x1ffffc;
  }
  channel->done();
}

