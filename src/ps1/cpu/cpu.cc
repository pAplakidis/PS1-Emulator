#include "cpu.h"

Cpu::Cpu(Interconnect *intercn){
  // PC reset value at the beginning of BIOS
  reg_pc = 0xbfc00000;

  // set $zero register value (index 0 in general purpose file) to 0x0
  registers[0] = 0x0;

  // reset values of other registers are unknown so set them to 0xdeadbeef
  for(int i=1;i<32;i++){
    registers[i] = 0xdeadbeef;
  }

  // TODO: this might not be for the CPU but for the whole ps1 (might have to move the files as well??)
  this->intercn = intercn;
}

// Get CPU register
uint32_t Cpu::reg(int idx){
  return registers[idx];
}

// Set CPU register
void Cpu::set_reg(int idx, uint32_t value){
  registers[idx] = value;
  registers[0] = 0x0; // make sure $zero/R0 is always 0
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
  uint32_t instr = load32(reg_pc);
  execute_instruction(instr);
  reg_pc += 4;  // this acts like a pointer but to C++ it is not (incrementing by 4 in a pseudo manual way)
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
      // there are commands with the same opcode but their last 6 bits are different from each other
      switch(instruction->instr & 0xffffffc0){
        case 0b100100:
        op_and(instruction);
        break;
      case 0b100000:
        op_add(instruction);
        break;
      case 0b100111:
        op_nor(instruction);
        break;
      case 0b100101:
        op_or(instruction);
        break;
      case 0b101010:
        op_slt(instruction);
        break;
      case 0b101011:
        op_sltu(instruction);
        break;
      case 0b100010:
        op_sub(instruction);
        break;
      default:
        printf("Unhandled instruction that belongs to the 000000 family %x\n", instr);
        exit(1);
      }
      break;
    case 0b001000:
      op_addi(instruction);
      break;
    case 0b001001:
      op_addiu(instruction);
      break;
    case 0b001100:
      op_andi(instruction);
      break;
    case 0b001111:
      op_lui(instruction);
      break;
    case 0b001101:
      op_ori(instruction);
      break;
    case 0b001010:
      op_slti(instruction);
      break;
    case 0b001011:
      op_sltiu(instruction);
      break;
    default:
      printf("Unhandled instruction %x\n", instr);
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
// TODO: code the rest of the instructions
// TODO: check for the signed and unsigned operations (need int32_T for signed and uint32_t for unsigned) (reg() returns unsigned so only worry about signed integers)

// ADD rd,rs,rt
// ADDU rd,rs,rt	
void Cpu::op_add(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();

  if((instruction->instr & 0xffffffc0) == 0b100000){
    int32_t sum = (int32_t)reg(rs) + (int32_t)reg(rt);
    set_reg(rd, sum);
  }
  else if((instruction->instr & 0xffffffc0) == 0b100001){
    uint32_t sum = reg(rs) + reg(rt);
    set_reg(rd, sum);
  }
  else{
    printf("Cannot recognised whether ADD is signed or unsigned");
    exit(1);
  }
}

// ADDI rt,rs,imm
void Cpu::op_addi(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();

  int32_t imm = instruction->immediate();
  int32_t sum = (int32_t)reg(rs) + imm;
  set_reg(rt, sum);
}

// ADDIU rt,rs,imm
void Cpu::op_addiu(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();

  uint32_t imm = instruction->immediate();
  uint32_t sum = reg(rs) + imm;
  set_reg(rt, sum);
}

// AND rd,rs,rt
void Cpu::op_and(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();

  uint32_t result = reg(rs) & reg(rt);
  set_reg(rd, result);
}

// ANDI rt,rs,imm
void Cpu::op_andi(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();

  uint32_t imm = instruction->immediate();
  uint32_t result = reg(rs) & imm;
  set_reg(rt, result);
}

// LUI rt,imm
void Cpu::op_lui(Instruction *instruction){
  // get register indices
  uint32_t rt = instruction->regt_idx();
  uint32_t imm = instruction->immediate();
  
  uint32_t result = imm << 16;
  set_reg(rt, result);
}

// NOR rd,rs,rt
void Cpu::op_nor(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();
  
  uint32_t result = ~(reg(rs) | reg(rt));
  set_reg(rd, result);
}

// OR rd,rs,rt
void Cpu::op_or(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();
  
  uint32_t result = reg(rs) | reg(rt);
  set_reg(rd, result);
}

// ORI rt,rs,imm
void Cpu::op_ori(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();

  uint32_t imm = instruction->immediate();
  uint32_t result = reg(rs) | reg(imm);
  set_reg(rt, result);
}

// SLT rd,rs,rt
void Cpu::op_slt(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();
  
  if((int32_t)reg(rs) < reg(rt))
    set_reg(rd, 1);
  else
    set_reg(rd, 0);
}

// SLTI rt,rs,imm
void Cpu::op_slti(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();

  uint32_t imm = instruction->immediate();
  if((int32_t)reg(rs) < imm)
    set_reg(rt, 1);
  else
    set_reg(rt, 0);
}

// SLTIU rt,rs,imm
void Cpu::op_sltiu(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();

  uint32_t imm = instruction->immediate();
  if(reg(rs) < imm)
    set_reg(rt, 1);
  else
    set_reg(rt, 0);
}

// SLTU rd,rs,rt
void Cpu::op_sltu(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();
  
  if(reg(rs) < reg(rt))
    set_reg(rd, 1);
  else
    set_reg(rd, 0);
}

// SUB rd,rs,rt
void Cpu::op_sub(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();
  
  int32_t result = (int32_t)reg(rs) - (int32_t)reg(rt);
  set_reg(rd, result);
}

