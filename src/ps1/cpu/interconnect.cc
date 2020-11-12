#include "interconnect.h"

Interconnect::Interconnect(Bios *bios){
  // TODO: delete this object from memory after done booting
  this->bios = bios;
}

// load a 32bit word at addr
uint32_t Interconnect::load32(uint32_t addr){
  uint32_t offset;
  
  if(offset = map::BIOS->contains(addr)){
    return bios->load32(offset);
  }
  else{
    printf("Cannot fetch address at %x", addr);
    exit(1);
  }
}

