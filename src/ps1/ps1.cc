#include "ps1.h"

Ps1::Ps1(std::string bios_path, std::string rom_path){
  bios = new Bios(bios_path);
  intercn = new Interconnect(bios);
  cpu = new Cpu(intercn);

  // Temporary tests
  cpu->load_rom(rom_path);
  cpu->main_loop();
}

