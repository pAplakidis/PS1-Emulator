#include "interconnect.h"

Interconnect::Interconnect(std::string bios_path){
  bios = new Bios(bios_path);
}

