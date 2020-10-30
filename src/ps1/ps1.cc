#include "ps1.h"

Ps1::Ps1(std::string bios_path, std::string rom_path){
  cpu = new Cpu(bios_path);
  intercn = new Interconnect();

  // Temporary tests
  cpu->load_rom(rom_path);
}

