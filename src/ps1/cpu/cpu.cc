#include "cpu.h"

Cpu::Cpu(){
  
}

// Loads ps1 rom into m_rom member variable
void Cpu::load_rom(std::string rom_path){
  std::vector<char> rom_data; // buffer for storing data from ROM

  // TODO: open rom_file, read data to rom_data, copy the rom_data to m_memory in the correct offset (according to the docs)
}

// Reads command in memory and executes it (also increases pc to point to next instruction)
void Cpu::cycle(){
  

}

void read_instruction(){
  
}

void execute_instruction(Instruction instr){
  switch(instr.opcode()){
  
  }
}

void branch(){
  
}

void read_word(){
  
}

void write_word(){
  
}

