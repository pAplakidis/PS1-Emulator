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

void Debugger::debug(Cpu *cpu){

}

