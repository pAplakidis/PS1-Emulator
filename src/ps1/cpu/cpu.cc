#include "cpu.h"

Cpu::Cpu(Interconnect *intercn){
  // PC reset value at the beginning of BIOS
  reg_pc = 0xbfc00000;

  // handles branching issues with pipelining
  next_instruction = new Instruction(0x0); // NOP

  // set $zero register value (index 0 in general purpose file) to 0x0
  registers[0] = 0x0;

  // set Cop0 status register
  sr = 0x0;

  // reset values of other registers are unknown so set them to 0xdeadbeef
  for(int i=1;i<32;i++){
    registers[i] = 0xdeadbeef;
  }

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
  Instruction *instruction = next_instruction;  // this solves branching issues with pipelining
  next_instruction = decode(load32(reg_pc));
  reg_pc += 4;  // this acts like a pointer but to C++ it is not (incrementing by 4 in a pseudo manual way)
  execute_instruction(instruction);
}

uint32_t Cpu::load32(uint32_t addr){
  return intercn->load32(addr);
}

// Store 32bit value into memory
void Cpu::store32(uint32_t addr, uint32_t val){
  intercn->store32(addr, val);
}

Instruction* Cpu::decode(uint32_t instr){
  Instruction *instruction = new Instruction(instr);
  return instruction;
}

void Cpu::execute_instruction(Instruction *instruction){
  // TODO: add all ~56 opcodes for this processor
  switch(instruction->opcode()){
    case 0b000000:
      // there are commands with the same opcode but their last 6 bits are different from each other
      switch(instruction->subfunction()){
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
        case 0b100110:
          op_xor(instruction);
          break;
        case 0b000000:
          op_sll(instruction);
          break;
        case 0b000100:
          op_sllv(instruction);
          break;
        case 0b000011:
          op_sra(instruction);
          break;
        case 0b000111:
          op_srav(instruction);
          break;
        case 0b000010:
          op_srl(instruction);
          break;
        case 0b000110:
          op_srlv(instruction);
          break;
        default:
          printf("Unhandled instruction that belongs to the 000000 family %x\n", instruction->instr);
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
    case 0b001110:
      op_xori(instruction);
      break;
    case 0b101011:
      op_sw(instruction);
      break;
    case 0b100011:
      op_lw(instruction);
      break;
    case 0b000010:
      op_j(instruction);
      break;
    case 0b010000:
      op_cop0(instruction);
      break;
    default:
      printf("Unhandled instruction %x\n", instruction->instr);
      exit(1);
  }
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

// TODO: need to check for overflow (in rust, rs.checked_add(imm))
// ADDI rt,rs,imm
void Cpu::op_addi(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();

  int32_t imm = instruction->imm_se();
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
// SUBU rd,rs,rt
void Cpu::op_sub(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();
  
  //int32_t diff = (int32_t)reg(rs) - (int32_t)reg(rt);
  //set_reg(rd, diff);

  if((instruction->instr & 0xffffffc0) == 0b100010){
    int32_t diff = (int32_t)reg(rs) - (int32_t)reg(rt);
    set_reg(rd, diff);
  }
  else if((instruction->instr & 0xffffffc0) == 0b100011){
    uint32_t diff = reg(rs) - reg(rt);
    set_reg(rd, diff);
  }
  else{
    printf("Cannot recognised whether ADD is signed or unsigned");
    exit(1);
  }
}

// XOR rd,rs,rt
void Cpu::op_xor(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();

  set_reg(rd, reg(rs)^reg(rt));
}

// XORI rt,rs,imm
void Cpu::op_xori(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();

  uint32_t imm = instruction->immediate();
  set_reg(rt, reg(rs)^imm);
}

// SW rt,offset(rs)
void Cpu::op_sw(Instruction *instruction){
  if(sr & 0x10000 != 0){
    // Cache is isolated, ignore write
    printf("Ignoring store while cache is isolated\n");
    return;
  }

  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  int32_t imm = instruction->imm_se();

  uint32_t addr = reg(rs) + imm;
  store32(addr, reg(rt));
}

// LW rt,offset(rs)
void Cpu::op_lw(Instruction *instruction){
  if(sr & 0x10000 != 0){
    // Cache is isolated, ignore write
    printf("Ignoring load while cache is isolated\n");
    return;
  }
  
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  int32_t imm = instruction->imm_se();

  uint32_t addr = reg(rs) + imm;
  set_reg(rt, load32(addr));
}

// SLL rd,rt,sa
void Cpu::op_sll(Instruction *instruction){
  // get register indices
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();
  int32_t sa = instruction->shift();

  set_reg(rd, reg(rt) << sa);
}

// SLLV rd,rt,rs
void Cpu::op_sllv(Instruction *instruction){
  // get register indices
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();
  uint32_t rs = instruction->regs_idx();

  set_reg(rd, reg(rt) << reg(rs));
}

// SRA rd,rt,sa
void Cpu::op_sra(Instruction *instruction){
  // get register indices
  int32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();
  int32_t sa = instruction->shift();

  set_reg(rd, reg(rt) >> sa);
}

// SRAV rd,rt,rs
void Cpu::op_srav(Instruction *instruction){
  // get register indices
  int32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();
  uint32_t rs = instruction->regs_idx();

  set_reg(rd, reg(rt) >> reg(rs));
}

// SRL rd,rt,sa
void Cpu::op_srl(Instruction *instruction){
  // get register indices
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();
  int32_t sa = instruction->shift();

  set_reg(rd, reg(rt) >> sa);
}

// SRLV rd,rt,rs
void Cpu::op_srlv(Instruction *instruction){
  // get register indices
  uint32_t rt = instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();
  uint32_t rs = instruction->regs_idx();

  set_reg(rd, reg(rt) >> reg(rs));
}

// TODO: need all branch instructions
// Branch to immediate value 'offset'
void Cpu::branch(uint32_t offset){
  // offset immediates are always shifted to the right by 2, since pc addresses need to be alogned on 32bits at all times (check MIPS architecture for that)
  offset = offset << 2;
  uint32_t pc = reg_pc;

  pc += offset;
  pc -= 4;  // we need to compensate for the hardcoded += offset in execute_instruction()
  reg_pc = pc;
}

// J target
void Cpu::op_j(Instruction *instruction){
  uint32_t target = instruction->imm_jump();

  reg_pc = (reg_pc & 0xf0000000) | (target << 2);
}

// BNE rs,rt,offset
void Cpu::op_bne(Instruction *instruction){
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  int32_t imm = instruction->imm_se();
  
  if(reg(rs) != reg(rt)){
    branch(imm);
  }
}

// TODO: code this
// MFC0 rt,rd
// Move from Coprocessor
void Cpu::op_mfc0(Instruction *instruction){

}

// MTC0 rt,rd
// Move to Coprocessor
void Cpu::op_mtc0(Instruction *instruction){
  // get register indices
  uint32_t cpu_r = instruction->regt_idx();
  uint32_t cop_r = instruction->regd_idx(); // TODO: in rust it is regd_idx().0, what is that?

  uint32_t v = reg(cpu_r);

  switch(cop_r){
    case 12:
      sr = v;
      break;
    default:
      printf("Unhandled cop0 register %x\n", cop_r);
      exit(1);
  }
}

// Coprocessor 0 opcode
void Cpu::op_cop0(Instruction *instruction){
  switch(instruction->cop_opcode()){
    case 0b00000:
      op_mfc0(instruction);
      break;
    case 0b00100:
      op_mtc0(instruction);
      break;
    default:
      printf("Unhandled cop0 isntruction %x\n", instruction->instr);
      exit(1);
  }
}

