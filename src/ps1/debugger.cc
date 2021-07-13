#include "debugger.h"

// Adds a breakpoint that will trigger when the instruction at 'addr' is about to be executed
void Debugger::add_breakpoint(uint32_t addr){
  // make sure the same address is not added twice
  if(!std::count(breakpoints.begin(), breakpoints.end(), addr)){
    breakpoints.push_back(addr);
  }
}

// Deletes breakpoint at 'addr' (does nothing if there was no breakpoint set for this address)
void Debugger::del_breakpoint(uint32_t addr){
  breakpoints.erase(std::find(breakpoints.begin(), breakpoints.end(), addr));
}

// Called by the CPU when it's aout to execute a new instruction. This function is called before all CPU instructions so it needs to be as fast as possible
void Debugger::pc_change(Cpu *cpu){
  if(std::count(breakpoints.begin(), breakpoints.end(), cpu->reg_pc)){
    debug(cpu);
  }
}

// Add a breakpoint that will trigger when the CPU attempts to read from 'addr'
void Debugger::add_read_watchpoint(uint32_t addr){
  // make sure we're not adding the same address twice
  if(!std::count(read_watchpoints.begin(), read_watchpoints.end(), addr)){
    read_watchpoints.push_back(addr);
  }
}

// Delete read watchpoint at 'addr', does nothing if there was no breakpoint set for this address
void Debugger::del_read_watchpoint(uint32_t addr){
  read_watchpoints.erase(std::find(read_watchpoints.begin(), read_watchpoints.end(), addr));
}

// Called by the CPU when it's about to load a value from memory
void Debugger::memory_read(Cpu *cpu, uint32_t addr){
  // TODO: need to handle unaligned watchpoints (for instance if we have a watchpoint on address 1 and the CPU executes a load32 at address 0, we should break, we might need to mask the region as well)
  if(!std::count(read_watchpoints.begin(), read_watchpoints.end(), addr)){
    printf("Read watchpoint triggered at 0x%8x\n", addr); // TODO: make all hex values 0x%8x for better readability
    debug(cpu);
  }
}

// Add a breakpoint that will trigger when the CPU attempts to write to 'addr'
void Debugger::add_write_watchpoint(uint32_t addr){
  // make sure we're not adding the same address twice
  if(!std::count(write_watchpoints.begin(), write_watchpoints.end(), addr)){
    write_watchpoints.push_back(addr);
  }
}

// Delete write watchpoint at 'addr', does nothing if there was no breakpoint set for this address
void Debugger::del_write_watchpoint(uint32_t addr){
  write_watchpoints.erase(std::find(write_watchpoints.begin(), write_watchpoints.end(), addr));
}

// Called by the CPU when it's about to write a value in memory
void Debugger::memory_write(Cpu *cpu, uint32_t addr){
  // TODO: same as read
  if(!std::count(write_watchpoints.begin(), write_watchpoints.end(), addr)){
    printf("Write watchpoint triggered at 0x%8x\n", addr);
    debug(cpu);
  }
}

void Debugger::debug(Cpu *cpu){

}

