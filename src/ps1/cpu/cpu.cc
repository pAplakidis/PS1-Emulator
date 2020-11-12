#include "cpu.h"

Cpu::Cpu(Interconnect *intercn){
  // PC reset value at the beginning of BIOS
  reg_pc = 0xbfc00000;

  // set 32 registers to 0x00000000
  reg_sp = 0;
  reg_ra = 0;
  reg_gp = 0;
  fp = 0;

  reg_k0 = 0;
  reg_k1 = 0;

  reg_v0 = 0;
  reg_v1 = 0;

  reg_a0 = 0;
  reg_a1 = 0;
  reg_a2 = 0;
  reg_a3 = 0;


  reg_s0 = 0;
  reg_s1 = 0;
  reg_s2 = 0;
  reg_s3 = 0;
  reg_s4 = 0;
  reg_s5 = 0;
  reg_s6 = 0;
  reg_s7 = 0;

  reg_t0 = 0;
  reg_t1 = 0;
  reg_t2 = 0;
  reg_t3 = 0;
  reg_t4 = 0;
  reg_t5 = 0;
  reg_t6 = 0;
  reg_t7 = 0;
  reg_t8 = 0;
  reg_t9 = 0;

  // TODO: this might not be for the CPU but for the whole ps1 (might have to move the files as well??)
  this->intercn = intercn;
}

// Loads ps1 rom into m_rom member variable
void Cpu::load_rom(std::string rom_path){
  // TODO: open rom_file, read data to rom_data, copy the rom_data to m_memory in the correct offset (according to the docs)
  std::ifstream instream(rom_path, std::ios::in | std::ios::binary);
  std::vector<uint8_t> temp_data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
  
  rom_data = temp_data;
}

// main loop of CPU
void Cpu::main_loop(){
  while(1){
    Cpu::cycle();
  }
}

// Reads command in memory and executes it (also increases pc to point to next instruction)
void Cpu::cycle(){
  // TODO: temp_pc might not be needed, we might need to increase reg_pc first and then execute instr
  uint32_t temp_pc = reg_pc;
  reg_pc += 4;  // this acts like a pointer but to C++ it is not (incrementing by 4 in a pseudo manual way)
  
  uint32_t instr = load32(reg_pc);
  execute_instruction(instr);
}

uint32_t Cpu::load32(uint32_t addr){
  return intercn->load32(addr);
}

Instruction* Cpu::decode(uint32_t instr){
  Instruction *instruction = new Instruction(instr);
  return instruction;
}

void Cpu::execute_instruction(uint32_t instr){
  Instruction *instruction = decode(instr);

  // TODO: add all ~56 opcodes for this processor
  switch(instruction->opcode()){
    case 0b000000:
      op_add(instruction);
      break;
    case 0b001000:
      op_addi(instruction);
      break;
    default:
      printf("Unhandled instruction %x", instr);
      exit(1);
  }
}

void Cpu::branch(){
  
}

void Cpu::read_word(){
  
}

void Cpu::write_word(){
  
}

// CPU instructions/operations

// ADD rd,rs,rt
void Cpu::op_add(Instruction *instruction){
  // NOTE: these are addresses
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();

  // TODO: load the values of the registers to the addresses and execute operation
}

// ADDI rt,rs,imm
void Cpu::op_addi(Instruction *instruction){

}

