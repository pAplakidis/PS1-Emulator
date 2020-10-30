#include <fstream>

#include "cpu/cpu.h"
#include "interconnect.h"

class Ps1{

public:
  Cpu *cpu;
  Interconnect *intercn;

  Ps1(std::string bios_path, std::string rom_path);
};

