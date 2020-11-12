#include <stdint.h>

#include "instruction.h"
#include "opcode.h"

Instruction::Instruction(uint32_t instr){
  this->instr = instr;
}

// returns opcode bits [31:26]
uint32_t Instruction::opcode(){
  return instr >> 26;
}

// TODO: are we skipping [25:20]???

// returns register index bits [20:16]
uint32_t Instruction::reg_idx(){
  return (instr >> 16) & 0x1f;
}

// returns immediate bits [16:0]
uint32_t Instruction::immediate(){
  return instr & 0xffff;
}

