#include "ps1.h"

Ps1::Ps1(std::ifstream boot, std::ifstream rom){
  this->m_boot = boot;
  this->m_rom = rom;

  this->m_cpu = new Cpu();
}

