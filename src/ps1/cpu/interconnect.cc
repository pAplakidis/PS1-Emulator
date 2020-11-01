#include "interconnect.h"

Interconnect::Interconnect(std::string bios_path){
  // TODO: delete this object from memory after done booting
  bios = new Bios(bios_path);
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

