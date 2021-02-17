#include <stdint.h>

#include "instruction.h"

Instruction::Instruction(uint32_t instr){
  this->instr = instr;
}

// returns opcode bits [31:26]
uint32_t Instruction::opcode(){
  return instr >> 26;
}

// returns subfunction bits [5:0]
uint32_t Instruction::subfunction(){
  return instr & 0x3f;
}

// returns coprocessor opcode bits [25:21]
uint32_t Instruction::cop_opcode(){
  return (instr >> 21) & 0x1f;
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

// returns unsigned immediate bits [15:0]
uint32_t Instruction::immediate(){
  return instr & 0xffff;
}

// returns signed immediate bits [15:0]
int32_t Instruction::imm_se(){
  return instr & 0xffff;
}

// shift immediate values are stored in bits [10:6]
uint32_t Instruction::shift(){
  return (instr >> 6) & 0x1f;
}

// jump target stored in bits [25:0]
uint32_t Instruction::imm_jump(){
  return instr & 0x3ffffff;
}

