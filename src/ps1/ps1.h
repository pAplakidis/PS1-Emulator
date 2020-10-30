#include <fstream>

#include "cpu/cpu.h"

class Ps1{

public:
  Cpu *cpu;

  Ps1(std::string bios_path, std::string rom_path);
};

