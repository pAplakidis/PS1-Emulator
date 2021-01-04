#include "interconnect.h"

Interconnect::Interconnect(Bios *bios){
  // TODO: delete this object from memory after done booting
  this->bios = bios;
}

// load a 32bit word at addr
uint32_t Interconnect::load32(uint32_t addr){
  uint32_t offset;

  // Check for address alignment (must be an address-multiple of 32bits)
  if(addr % 4 != 0){
    printf("Unaligned load32 address: %x", addr);
    exit(1);
  }
  
  if(offset = map::BIOS->contains(addr)){
    return bios->load32(offset);
  }
  else{
    printf("Cannot fetch address at %x", addr);
    exit(1);
  }
}

// TODO: the only peripheral we support right now is BIOS ROM and we can't write to it, come back and complete this later
// store 32bit word val into addr
void Interconnect::store32(uint32_t addr, uint32_t val){
  // Check for address alignment (must be an address-multiple of 32bits)
  if(addr % 4 != 0){
    printf("Unaligned store32 address: %x", addr);
    exit(1);
  }

  printf("Unhandled store32 into address %x", addr);
  exit(1);
}

