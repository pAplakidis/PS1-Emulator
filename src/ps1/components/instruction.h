#pragma once

class Instruction{
public:
  uint32_t instr;

  Instruction(uint32_t instr);
  uint32_t opcode();
  uint32_t subfunction();
  uint32_t cop_opcode();
  uint32_t regs_idx();
  uint32_t regt_idx();
  uint32_t regd_idx();
  uint32_t immediate();
  int32_t imm_se();
  uint32_t shift();
  uint32_t imm_jump();
};

