#pragma once

#include <vector>
#include <algorithm>
#include <stdint.h>

#include "components/cpu.h"

class Debugger{
public:
  // Vector containing all active read watchpoints
  std::vector<uint32_t> read_watchpoints;
  // Vector containing all active write watchpoints
  std::vector<uint32_t> write_watchpoints;

  // Vector containing all active breakpoint addresses
  std::vector<uint32_t> breakpoints;

  void add_breakpoint(uint32_t addr);
  void del_breakpoint(uint32_t addr);
  void pc_change(Cpu *cpu);
  void add_read_watchpoint(uint32_t addr);
  void del_read_watchpoint(uint32_t addr);
  void memory_read(Cpu *cpu, uint32_t addr);
  void add_write_watchpoint(uint32_t addr);
  void del_write_watchpoint(uint32_t addr);
  void memory_write(Cpu *cpu, uint32_t addr);

  void debug(Cpu *cpu);
};

