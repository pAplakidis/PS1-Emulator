#include <fstream>

#include "cpu/cpu.h"

class Ps1{

public:
  Cpu *m_cpu;

  Ps1(std::string boot_path, std::string rom_path);
};

