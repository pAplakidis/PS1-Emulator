#include <stdint.h>

#include "instruction.h"
#include "opcode.h"

Instruction::Instruction(uint32_t instr){
  this->instr = instr;
}

uint32_t Instruction::opcode(){
  return 0x0;
}

