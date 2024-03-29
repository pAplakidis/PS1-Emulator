#include "interconnect.h"

Interconnect::Interconnect(Bios *bios, Gpu *gpu){
  this->bios = bios;
  ram = new Ram();

  // DMA registers
  dma = new Dma();

  // Gpu
  this->gpu = gpu;
}

// TODO: need to ad RAM_SIZE CHECK AS WELL (we also get a SEG FAULT sometimes)
// load a 32bit word at addr
uint32_t Interconnect::load32(uint32_t addr){
  // Check for address alignment (must be an address-multiple of 32bits)
  if(addr % 4 != 0){
    printf("%sUnaligned load32 address: 0x%8x%s\n", color::red, addr, color::white);
    exit(1);
  }
  
  uint32_t abs_addr = map::mask_region(addr);

  if(uint32_t *offset = map::RAM->contains(abs_addr)){
    uint32_t ret = ram->load32(*offset);
    free(offset);
    offset = NULL;
    return ret;
  }

  // TODO: implement for store as well
  // TODO: implement for new ranges
  // TODO: implement for 16 and 8 bits as well
  if(uint32_t *offset = map::SCRATCH_PAD.contains(abs_addr)){
    if(addr > 0xa0000000){
      printf("ScratchPad access through uncached memory");
      exit(1);
    }
  }

  if(uint32_t *offset = map::PAD_MEMCARD.contains(abs_addr)){
    printf("%sUnhandled load to PAD_MEMCARD%s\n", color::red, color::white);
    exit(1);
  }

  /*
  if(uint32_t *offset = map::RAM_KSEG0->contains(abs_addr)){
    uint32_t ret = ram->load32(*offset);
    free(offset);
    offset = NULL;
    //exit(1);
    return ret;
  }
  */

  if(uint32_t *offset = map::BIOS->contains(addr)){
    uint32_t ret = bios->load32(*offset);
    free(offset);
    offset = NULL;
    return ret;
  }

  if(uint32_t *offset = map::IRQ_CONTROL->contains(abs_addr)){
    printf("IRQ control read 0x%x\n", *offset);
    free(offset);
    offset = NULL;
    return 0;
  }

  if(uint32_t *offset = map::DMA->contains(abs_addr)){
    printf("DMA read 0x%08x\n", abs_addr);
    free(offset);
    offset = NULL;
    return 0;
  }

  if(uint32_t *offset = map::GPU->contains(abs_addr)){
    printf("GPU read 0x%x\n", *offset);
    switch(*offset){
      // GPUSTAT: set bit 28 to signal that the GPU is ready to receive DMA blocks
      case 4:
        free(offset);
        offset = NULL;
        return 0x10000000;
      default:
        return 0;
    }
  }

  if(uint32_t *offset = map::DMA->contains(abs_addr)){
    uint32_t ret = dma_reg(*offset);
    free(offset);
    offset = NULL;
    return ret;
  }

  if(uint32_t *offset = map::GPU->contains(abs_addr)){
    switch(*offset){
      // GPUSTAT: set bit 26, 27, 28 tosignal that the GPU is ready for DMA and CPU access.
      // This way the BIOS won't dead lock waiting for an event that will never come.
      case 4:
        free(offset);
        offset = NULL;
        return 0x1c000000;
      default:
        free(offset);
        offset = NULL;
        return 0;
    }
  }

  printf("%sUnhandled load32 at address 0x%08x\tabs_addr: 0x%08x%s\n", color::red, addr, abs_addr, color::white);
  exit(1);
}

// Load 16bit halfword at 'addr'
uint16_t Interconnect::load16(uint32_t addr){
  if(addr % 2 != 0){
    printf("Unaligned load16 address: %8x\n", addr);
    exit(1);
  }

  uint32_t abs_addr = map::mask_region(addr);

  if(uint32_t *offset = map::SPU->contains(abs_addr)){
    printf("%sUnhandled read from SPU register 0x%08x%s\n", color::red, abs_addr, color::white);
    free(offset);
    offset = NULL;
    return 0;
  }

  if(uint32_t *offset = map::RAM->contains(abs_addr)){
    uint16_t ret = ram->load16(*offset);
    free(offset);
    offset = NULL;
    return ret;
  }

  if(uint32_t *offset = map::IRQ_CONTROL->contains(abs_addr)){
    printf("IRQ control read 0x%x\n", *offset);
    free(offset);
    offset = NULL;
    return 0;
  }

  printf("%sUnhandled load16 at address 0x%08x%s\n", color::red, addr, color::white);
  exit(1);
}

// load byte at 'addr'
uint8_t Interconnect::load8(uint32_t addr){
  uint32_t abs_addr = map::mask_region(addr);
  
  if(uint32_t *offset = map::RAM->contains(abs_addr)){
    uint8_t ret = ram->load8(*offset);
    free(offset);
    offset = NULL;
    return ret;
  }

  if(uint32_t *offset = map::BIOS->contains(abs_addr)){
    uint8_t ret = bios->load8(*offset);
    free(offset);
    offset = NULL;
    return ret;
  }

  if(uint32_t *_ = map::EXPANSION_1->contains(abs_addr)){
    // no expansion implemented
    free(_);
    _ = NULL;
    return 0xff;
  }

  printf("%sUnhandled load8 at address 0x%8x%s\n", color::red, addr, color::white);
  exit(1);
}

// TODO: the only peripheral we support right now is BIOS ROM and we can't write to it, come back and complete this later
// store 32bit word val into addr
void Interconnect::store32(uint32_t addr, uint32_t val){
  // Check for address alignment (must be an address-multiple of 32bits)
  if(addr % 4 != 0){
    printf("Unaligned store32 address: 0x%x\n", addr);
    exit(1);
  }

  uint32_t abs_addr = map::mask_region(addr);

  // Handle Expansion mapping
  if(uint32_t *offset = map::MEMCONTROL->contains(addr)){
    switch(*offset){
      // Expansion 1 base address
      case 0:
        if(val != 0xf1000000){
          printf("%sBad Expansion 1 base address: 0x%x%s\n", color::red, val, color::white);
          exit(1);
        }
      case 4:
        if(val != 0xf1802000){
          printf("%sBad Expansion 2 base address: 0x%x%s\n", color::red, val, color::white);
          exit(1);
      }
      default:
        printf("%sUnhandled write to MEMCONTROL register%s\n", color::red, color::white);
        free(offset);
        offset = NULL;
        break;
    }
    return;
  }

  if(uint32_t *offset = map::IRQ_CONTROL->contains(abs_addr)){
    printf("IRQ_control: 0x%x <- 0x%08x\n", *offset, val);
    free(offset);
    offset = NULL;
    return;
  }

  if(uint32_t *offset = map::DMA->contains(abs_addr)){
    printf("DMA write: 0x%08x: 0x%08x\n", abs_addr, val);
    free(offset);
    offset = NULL;
    return;
  }

  if(uint32_t *offset = map::GPU->contains(abs_addr)){
    printf("GPU write 0x%x: 0x%08x\n", *offset, val);
    free(offset);
    offset = NULL;
    return;
  }

  if(uint32_t *offset = map::TIMERS->contains(abs_addr)){
    printf("%sUnhandled write to timer register 0x%08x: 0x%08x%s\n", color::red, *offset, val, color::white);
    free(offset);
    offset = NULL;
    return;
  }

  if(uint32_t *offset = map::RAM_SIZE->contains(abs_addr)){
    printf("\n%sUnhandled write to ram_size register 0x%08x: 0x%08x%s\n", color::red, *offset, val, color::white);
    free(offset);
    offset = NULL;
    return;
  }

  if(uint32_t *offset = map::DMA->contains(abs_addr)){
    return set_dma_reg(*offset, val);
    free(offset);
    offset = NULL;
  }

  if(uint32_t *offset = map::GPU->contains(abs_addr)){
    switch(*offset){
      case 0:
        gpu->gp0(val);
        free(offset);
        offset = NULL;
        break;
      default:
        printf("GPU write 0x%x: 0x%08x\n", *offset, val);
        exit(1);
    }
    return;
  }

  printf("%sUnhandled store32 at address 0x%08x <- 0x%x%s\n", color::red, addr, val, color::white);
  exit(1);
}

// TODO: the only peripheral we support right now is BIOS ROM and we can't write to it, come back and complete this later
// Store 16bit halfword 'val' into 'addr'
void Interconnect::store16(uint32_t addr, uint16_t val){
  if(addr % 2 != 0){
    printf("Unaligned store16 address 0x%08x\n", addr);
    exit(1);
  }

  uint32_t abs_addr = map::mask_region(addr);

  if(uint32_t *offset = map::RAM->contains(abs_addr)){
    ram->store16(*offset, val);
    free(offset);
    offset = NULL;
    return;
  }

  if(uint32_t *offset = map::SPU->contains(abs_addr)){
    printf("%sUnhandled write to SPU register 0x%x%s\n", color::red, *offset, color::white);
    free(offset);
    offset = NULL;
    return;
  }
  
  if(uint32_t *offset = map::TIMERS->contains(abs_addr)){
    printf("%sUnhandled write to timer register 0x%x%s\n",  color::red, *offset, color::white);
    return;
  }

  if(uint32_t *offset = map::IRQ_CONTROL->contains(abs_addr)){
    printf("IRQ control write 0x%x: 0x%04x\n", *offset, val);
    free(offset);
    offset = NULL;
    return;
  }

  printf("%sUnhandled store16 at address 0x%08x <- 0x%x\n", color::red, addr, val, color::white);
  exit(1);
}

// TODO: the only peripheral we support right now is BIOS ROM and we can't write to it, come back and complete this later
// Store 8bit halfword 'val' into 'addr'
void Interconnect::store8(uint32_t addr, uint8_t val){
  uint32_t abs_addr = map::mask_region(addr);

  if(uint32_t *offset = map::EXPANSION_2->contains(abs_addr)){
    printf("%sUnhandled write to SPU register 0x%x%s\n", color::red, *offset, color::white);
    ram->store8(*offset, val);
    free(offset);
    offset = NULL;
    return;
  }
  
  printf("%sUnhandled store8 into address 0x%08x <- 0x%x%s\n", color::red, addr, val, color::white);
  exit(1);
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
      printf("%sUnhandled DMA access: 0x%08x%s\n", color::red, *offset, color::red);
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
          printf("%sUnhandled DMA read at 0x%x%s\n", color::red, offset, color::white);
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
          printf("%sUnhandled DMA read at 0x%x%s\n", color::red, offset, color::white);
          exit(1);
      }
      break;
    }
    default:
    {
      printf("%sUnhandled DMA read at 0x%x%s\n", color::red, offset, color::white);
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
      printf("%sUnhandled DMA write access: 0x%08x <- 0x%08x%s\n", color::red, offset, value, color::white);
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
          printf("%sUnhandled DMA write at 0x%x <- 0x%08x%s\n", color::red, offset, value, color::white);
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
          printf("%sUnhandled DMA write at 0x%x <- 0x%08x%s\n", color::red, offset, value, color::white);
          exit(1);
      }

      active_port = NULL;
      break;
    }
    default:
      printf("%sUnhandled DMA write at 0x%x <- 0x%08x%s\n", color::red, offset, value, color::white);
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

// Emulate DMA transfer for Manual and Request synchronization modes
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
      {
        uint32_t src_word = ram->load32(cur_addr);
        switch(port){
          case GPU:
            gpu->gp0(src_word);
            break;
          default:
            printf("%sUnhandled DMA destination port 0x%x%s\n", color::red, (uint8_t)port, color::white);
            exit(1);
        }
      }
      break;
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
            printf("%sUnhandled DMA source port 0x%x%s\n", color::red, (uint8_t)port), color::white;
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
  if(port != GPU){
    printf("Attempted linked list DMA on port 0x%x\n", (uint8_t)port);
    exit(1);
  }

  while(1){
    // In linked list mode, each entry starts with a "header" word. The high byte contains the number of words in the "packet" (not counting the header word)
    uint32_t header = ram->load32(addr);
    uint32_t remsz = header >> 24;
    
    while(remsz > 0){
      addr = (addr + 4) & 0x1ffffc;
      uint32_t command = ram->load32(addr);
      // Send command to the GPU
      gpu->gp0(command);
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

