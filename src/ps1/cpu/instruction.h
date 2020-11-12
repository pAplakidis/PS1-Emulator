#include <stdint.h>

class Instruction{
public:
  uint32_t instr;

  Instruction(uint32_t instr);
  uint32_t opcode();
};

