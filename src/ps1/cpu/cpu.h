#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "instruction.h"
#include "interconnect.h"

class Cpu{

// TODO: put private functions and variables here
private:
  // General Purpose Registers (check docs for their idx->name)
  uint32_t registers[32];

public:
  // 4KB instruction memory, 1KB data cache
  static const uint32_t MEMORY_SIZE = 512 * 1024; // CHECK THAT
  unsigned char m_memory[MEMORY_SIZE];  // main memory of the CPU

  // NOTE: 1 register here might not be needed (32 registers needed in the MIPS register_file, we have 33)
  // Special Purpose Registers
  uint32_t reg_pc;  // Program Counter (not inside register file)
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
  void store32(uint32_t addr, uint32_t val);

  Instruction* decode(uint32_t intruction);
  void execute_instruction(uint32_t instr);

  // Instructions

  // ALU
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

  // Memory Access
  void op_sw(Instruction *instruction);

 // Shifter
 void op_sll(Instruction *instruction);
 void op_sllv(Instruction *instruction);
 void op_sra(Instruction *instruction);
 void op_srav(Instruction *instruction);
 void op_srl(Instruction *instruction);
 void op_srlv(Instruction *instruction);

 // Branch
 void op_j(Instruction *instruction);;
};

