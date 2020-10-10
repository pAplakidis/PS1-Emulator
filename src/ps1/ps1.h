#include "cpu/cpu.h"

class Ps1{

public:
  Cpu *m_cpu;
  std::ifstream m_boot;
  std::ifstream m_rom;

  Ps1(std::ifstream boot, std::ifstream rom);
};

