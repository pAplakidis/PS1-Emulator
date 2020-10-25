#include "ps1.h"
#include "interconnect.h"

Ps1::Ps1(std::string boot_path, std::string rom_path){
  m_cpu = new Cpu();

  // Temporary tests
  m_cpu->load_rom(rom_path);
}

