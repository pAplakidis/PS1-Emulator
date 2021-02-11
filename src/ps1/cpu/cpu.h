#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "instruction.h"
#include "interconnect.h"
#include "helpers.h"

// Exception types (as stored in the "CAUSE" register)
enum Exception{
  // System call (caused by the SYSCALL opcode)
  SysCall = 0x8,
  // Arithmetic Overflow
  Overflow = 0xc,
  // Address error on load
  LoadAddressError = 0x4,
  // Address error on store
  StoreAddressError = 0x5
};

class Cpu{

public:
  // 4KB instruction memory, 1KB data cache
  static const uint32_t MEMORY_SIZE = 512 * 1024; // CHECK THAT
  unsigned char m_memory[MEMORY_SIZE];  // main memory of the CPU

  // General Purpose Registers (check docs for their idx->name)
  uint32_t registers[32];

  // 2nd set of registers used to emulate the load delay slot accurately, they contain the output of the current instruction
  uint32_t out_regs[32];

  // load initiated by the current instruction
  uint32_t load[2];

  // Cop0 register 12: Status Register
  uint32_t sr;

  // HI register (devision remainder, multiplication high result)
  uint32_t hi;
  // LO register (devision quotient, multiplication low result)
  uint32_t lo;

  // NOTE: 1 register here might not be needed (32 registers needed in the MIPS register_file, we have 33)
  // Special Purpose Registers
  uint32_t reg_pc;  // Program Counter (not inside register file)

  // for exception handling
  uint32_t next_pc;
  uint32_t current_pc;  // address of instruction currently being executed, used for setting the EPC in exceptions
  uint32_t cause; // Cop0 register 13: Cause Register
  uint32_t epc;   // Cop0 register 14: EPC
  
  bool branched;  // set by the current instruction if a branch occured and the next instruction will be in the delay slot
  bool delay_slot;  // set if the current instruction executes in the delay slot

  uint32_t reg_hi, reg_lo; // high and low 32bits of multiplication result (remainder of division for hi, quotient of division for lo)

  // other data variables
  std::vector<uint8_t> rom_data;

  // CPU object variables
  Interconnect *intercn;

  // Basic Functions
  Cpu(Interconnect *intercn);
  uint32_t reg(int idx);
  void set_reg(int idx, uint32_t value);
  void main_loop();
  void load_rom(std::string rom_path);
  void cycle();

  uint32_t load32(uint32_t addr);
  uint16_t load16(uint32_t addr);
  uint8_t load8(uint32_t addr);
  void store32(uint32_t addr, uint32_t val);
  void store16(uint32_t addr, uint16_t val);
  void store8(uint32_t addr, uint8_t val);

  void exception(enum Exception cause);

  Instruction* decode(uint32_t intruction);
  void execute_instruction(Instruction *instruction);

  // Instructions
  // basic ALU
  void op_add(Instruction *instruction);
  void op_addi(Instruction *instruction);
  void op_addiu(Instruction *instruction);
  void op_and(Instruction *instruction);
  void op_andi(Instruction *instruction);
  void op_lui(Instruction *instruction);
  void op_nor(Instruction *instruction);
  void op_or(Instruction *instruction);
  void op_ori(Instruction *instruction);
  void op_slt(Instruction *instruction);
  void op_slti(Instruction *instruction);
  void op_sltiu(Instruction *instruction);
  void op_sltu(Instruction *instruction);
  void op_sub(Instruction *instruction);
  void op_xor(Instruction *instruction);
  void op_xori(Instruction *instruction);

  // Multiply
  void op_div(Instruction *instruction);
  void op_divu(Instruction *instruction);
  void op_mflo(Instruction *instruction);
  void op_mfhi(Instruction *instruction);
  void op_mtlo(Instruction *instruction);
  void op_mthi(Instruction *instruction);

  // Memory Access
  void op_sw(Instruction *instruction);
  void op_sh(Instruction *instruction);
  void op_sb(Instruction *instruction);
  void op_lw(Instruction *instruction);
  void op_lb(Instruction *instruction);
  void op_lbu(Instruction *instruction);
  void op_lh(Instruction *instruction);
  void op_lhu(Instruction *instruction);

  // Shifter
  void op_sll(Instruction *instruction);
  void op_sllv(Instruction *instruction);
  void op_sra(Instruction *instruction);
  void op_srav(Instruction *instruction);
  void op_srl(Instruction *instruction);
  void op_srlv(Instruction *instruction);

  // Branch
  void branch(uint32_t offset);
  void op_j(Instruction *instruction);;
  void op_jal(Instruction *instruction);
  void op_jr(Instruction *instruction);
  void op_jalr(Instruction *instruction);
  void op_bne(Instruction *instruction);
  void op_beq(Instruction *instruction);
  void op_bgtz(Instruction *instruction);
  void op_blez(Instruction *instruction);
  void op_bxx(Instruction *instruction);
  void op_syscall(Instruction *instruction);

  // Coprocessors
  void op_cop0(Instruction *instruction);
  void op_mfc0(Instruction *instruction);
  void op_mtc0(Instruction *instruction);
  void op_rfe(Instruction *instruction);
};

