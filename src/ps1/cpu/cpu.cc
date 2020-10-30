#include "cpu.h"

Cpu::Cpu(std::string bios_path){
  // PC reset value at the beginning of BIOS
  reg_pc = 0xbfc00000;

  // set 32 registers to 0x00000000
  reg_sp = 0;
  reg_ra = 0;
  reg_gp = 0;
  fp = 0;

  reg_k0 = 0;
  reg_k1 = 0;

  reg_v0 = 0;
  reg_v1 = 0;

  reg_a0 = 0;
  reg_a1 = 0;
  reg_a2 = 0;
  reg_a3 = 0;


  reg_s0 = 0;
  reg_s1 = 0;
  reg_s2 = 0;
  reg_s3 = 0;
  reg_s4 = 0;
  reg_s5 = 0;
  reg_s6 = 0;
  reg_s7 = 0;

  reg_t0 = 0;
  reg_t1 = 0;
  reg_t2 = 0;
  reg_t3 = 0;
  reg_t4 = 0;
  reg_t5 = 0;
  reg_t6 = 0;
  reg_t7 = 0;
  reg_t8 = 0;
  reg_t9 = 0;
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

