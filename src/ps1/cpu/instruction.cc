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

// returns rs index bits [25:21]
uint32_t Instruction::regs_idx(){
  return (instr >> 21) & 0x1f;
}

// returns rt index bits [20:16]
uint32_t Instruction::regt_idx(){
  return (instr >> 16) & 0x1f;
}

// returns rd index bits [16:11]
uint32_t Instruction::regd_idx(){
  return (instr >> 11) & 0x1f;
}

// NOTE: there are 5 + 6 more bits that are not used (when we have immediate, they merge with rd to give 16 bits for it)

// returns immediate bits [15:0]
uint32_t Instruction::immediate(){
  return instr & 0xffff;
}

