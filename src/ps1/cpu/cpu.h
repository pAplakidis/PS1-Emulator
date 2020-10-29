#include <fstream>
#include <vector>
#include <stdint.h>

#include "instruction.h"
#include "bios.h"

class Cpu{

public:
  // 4KB instruction memory, 1KB data cache
  static const uint32_t MEMORY_SIZE = 5120; // CHECK THAT
  unsigned char m_memory[MEMORY_SIZE];

  // NOTE: 1 register here might not be needed (32 registers needed in the MIPS register_file, we have 33)
  // Registers
  uint32_t reg_pc;  // Program Counter (not inside register file)

  // 0-31 32bit registers
  uint32_t reg_sp;  // Stack Pointer
  uint32_t reg_ra;  // Return Address
  uint32_t reg_gp;  // global pointer (easy access to static or extern variables)
  uint32_t fp;      // 9th register variable (also named s8), subroutines which need one can use this as a "frame pointer"
  uint32_t reg_k0, reg_k1;  // reserved for use by interrupt/ trap handler

  // subroutine registers
  uint32_t reg_v0, reg_v1;                  // return values for subroutines
  uint32_t reg_a0, reg_a1, reg_a2, reg_a3;  // arguments for subroutines
  uint32_t reg_s0, reg_s1, reg_s2, reg_s3, reg_s4, reg_s5, reg_s6, reg_s7;  // subroutine register variables

  // general purpose registers
  static const uint32_t reg_0 = 0;
  uint32_t reg_t0, reg_t1, reg_t2, reg_t3, reg_t4, reg_t5, reg_t6, reg_t7, reg_t8, reg_t9;



  Cpu();
  void load_rom(std::string rom_path);
  void cycle();
  Instruction* read_instruction(uint32_t addr);
  void execute_instruction(Instruction *instr);
  void branch();
  void read_word();
  void write_word();

};

