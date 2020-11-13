
class Instruction{
public:
  uint32_t instr;

  Instruction(uint32_t instr);
  uint32_t opcode();
  uint32_t regs_idx();
  uint32_t regt_idx();
  uint32_t regd_idx();
  uint32_t immediate();
};

