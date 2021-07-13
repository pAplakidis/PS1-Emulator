#pragma once

#include <vector>
#include <algorithm>
#include <stdint.h>

#include "components/cpu.h"

class Debugger{
public:
  // Vector containing all active breakpoint addresses
  std::vector<uint32_t> breakpoints;

  void add_breakpoint(uint32_t addr);
  void del_breakpoint(uint32_t addr);
  void pc_change(Cpu *cpu);
  void debug(Cpu *cpu);
};

