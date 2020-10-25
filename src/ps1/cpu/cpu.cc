#include "cpu.h"

Cpu::Cpu(){
  // TODO: set a default value for the PC and other registers
}

// Loads ps1 rom into m_rom member variable
void Cpu::load_rom(std::string rom_path){
  std::vector<char> rom_data; // buffer for storing data from ROM

  // TODO: open rom_file, read data to rom_data, copy the rom_data to m_memory in the correct offset (according to the docs)
}

// Reads command in memory and executes it (also increases pc to point to next instruction)
void Cpu::cycle(){
  uint32_t temp_pc = reg_pc;
  reg_pc += 4;  // this acts like a pointer but to C++ it is not (incrementing by 4 in a pseudo manual way)
  
  Instruction *instr = read_instruction(temp_pc);
  execute_instruction(instr);
}

// TODO: get and decode instruction in addr of PC and return the object with the opcode
Instruction* Cpu::read_instruction(uint32_t addr){
  Instruction *instr = new Instruction();
  
  return instr;
}

void Cpu::execute_instruction(Instruction *instr){
  switch(instr->opcode()){
  
  }
}

void Cpu::branch(){
  
}

void Cpu::read_word(){
  
}

void Cpu::write_word(){
  
}

