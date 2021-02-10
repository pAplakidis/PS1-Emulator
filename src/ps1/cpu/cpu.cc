#include "cpu.h"

Cpu::Cpu(Interconnect *intercn){
  // PC reset value at the beginning of BIOS
  reg_pc = 0xbfc00000;
  next_pc = reg_pc + 4;

  // set $zero register value (index 0 in general purpose file) to 0x0
  registers[0] = 0x0;
  out_regs[0] = 0x0;  // if no load is pending we target $zero (does nothing)

  // for load delay slots
  load[0] = 0;
  load[1] = 0x0;

  // set Cop0 status register
  sr = 0x0;

  // reset values of other registers are unknown so set them to 0xdeadbeef
  for(int i=1;i<32;i++){
    registers[i] = 0xdeadbeef;
  }
  hi = 0xdeadbeef;
  lo = 0xdeadbeef;

  this->intercn = intercn;
}

// Get CPU register
uint32_t Cpu::reg(int idx){
  return registers[idx];
}

// Set CPU register
void Cpu::set_reg(int idx, uint32_t value){
  //registers[idx] = value;
  //registers[0] = 0x0; // make sure $zero/R0 is always 0
  out_regs[idx] = value;
  out_regs[0] = 0x0;  // make sure $zero/R0 is always 0
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
  // Fetch instruction at PC
  Instruction *instruction = decode(load32(reg_pc));  // this solves branching issues with pipelining

  // Increment next PC to point to the next instruction
  current_pc = reg_pc;
  reg_pc = next_pc;
  next_pc += 4;

  // execute the pending load (if any, otherwise it will load $zero which is a NOP)
  // set_reg works only on out_regs so this operation won't be visible by the next instruction
  int reg = (int)load[0];
  uint32_t val = load[1];
  set_reg(reg, val);

  // reset the load to target register 0 for the next instruction
  load[0] = 0;
  load[1] = 0x0;

  // executed decoded instruction
  execute_instruction(instruction);

  // copy output regs as input for next instruction
  for(int i=0; i < 32; i++){
    registers[i] = out_regs[i];
  }
}

// load 32bit value from the memory
uint32_t Cpu::load32(uint32_t addr){
  return intercn->load32(addr);
}

// load 8bit value from the memory
uint8_t Cpu::load8(uint32_t addr){
  return intercn->load8(addr);
}

// Store 32bit value into memory
void Cpu::store32(uint32_t addr, uint32_t val){
  intercn->store32(addr, val);
}

// Store 16bit value into memory
void Cpu::store16(uint32_t addr, uint16_t val){
  intercn->store16(addr, val);
}

// Store 8bit value into memory
void Cpu::store8(uint32_t addr, uint8_t val){
  intercn->store8(addr, val);
}

Instruction* Cpu::decode(uint32_t instr){
  Instruction *instruction = new Instruction(instr);
  return instruction;
}

// Trigger an exception
void Cpu::exception(enum Exception cause){
  // Exception handler address depends on the "BEV"
  uint32_t handler;
  switch(sr & (1 << 22) != 0){
    case true:
      handler = 0xbfc00180;
      break;
    case false:
      handler = 0x80000080;
      break;
  }

  // Shift bits [5:0] of "SR" 2 places to the left
  // Those bits are 3 pairs of Interrupt Enable User Mode bits behaving like a stack 3 entries deep
  // Entering an exception pushes a pair of zeroes by left shifting the stack which disables interrupts and puts the CPU in kernel mode
  // The original third entry is discarded (it's up to the kernel to handle more than two recursive exception levels)
  uint32_t mode = sr & 0x3f;
  sr &= 0x3f;
  sr |= (mode << 2) & 0x3f;

  // Update "CAUSE" register with the exception code (bits [6:2])
  this->cause = (uint32_t)cause << 2;

  // Save the current instruction address in "EPC";
  epc = current_pc;

  // Exceptions don't have a branch delay, we jump directly into the handler
  reg_pc = handler;
  next_pc = reg_pc + 4;
}

void Cpu::execute_instruction(Instruction *instruction){
  // TODO: add all ~56 opcodes for this processor (check if some are not in the switch statement)
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
        case 0b001000:
          op_jr(instruction);
          break;
        case 0b001001:
          op_jalr(instruction);
          break;
        case 0b011010:
          op_div(instruction);
          break;
        case 0b011011:
          op_divu(instruction);
          break;
        case 0b010010:
          op_mflo(instruction);
          break;
        case 0b010000:
          op_mfhi(instruction);
          break;
        case 0b001100:
          op_syscall(instruction);
          break;
        default:
          printf("Unhandled instruction subfunction(belongs to the 000000 family) %x\n", instruction->instr);
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
    case 0b101001:
      op_sh(instruction);
      break;
    case 0b101000:
      op_sb(instruction);
      break;
    case 0b100011:
      op_lw(instruction);
      break;
    case 0b100000:
      op_lb(instruction);
      break;
    case 0b100100:
      op_lbu(instruction);
      break;
    case 0b000010:
      op_j(instruction);
      break;
    case 0b000011:
      op_jal(instruction);
      break;
    case 0b000101:
      op_bne(instruction);
      break;
    case 0b000100:
      op_beq(instruction);
      break;
    case 0b000111:
      op_bgtz(instruction);
      break;
    case 0b000110:
      op_blez(instruction);
      break;
    case 0b000001:
      op_bxx(instruction);
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

    // TODO: check for ADD overflow (match s.checked_add(t) in Rust)
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
  
  if((int32_t)reg(rs) < (int32_t)reg(rt))
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

// TODO: more multiplication commands to be handled
// DIV rs,rt
void Cpu::op_div(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();

  int32_t n = (int32_t)reg(rs);
  int32_t d = (int32_t)reg(rt);

  // Division in MIPS has some special cases
  if(d == 0){
    // division by 0, results are bogus
    hi = (uint32_t)n;

    if(n >= 0){
      lo = 0xffffffff;
    }else{
      lo = 1;
    }
  }else if((uint32_t)n == 0x80000000 && d == -1){
    // Result is not representable in a 32bit signed integer
    hi = 0;
    lo = 0x80000000;
  }else{
    hi = (uint32_t)(n % d);
    lo = (uint32_t)(n / d);
  }
}

// DIVU rs,rt
void Cpu::op_divu(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();

  uint32_t n = reg(rs);
  uint32_t d = reg(rt);
  
  // Division in MIPS has some special cases
  if(d == 0){
    // division by 0, results are bogus
    hi = n;
    lo = 0xffffffff;
  }else{
    hi = n % d;
    lo = n / d;
  }
}

// MFLO rd (move from LO)
void Cpu::op_mflo(Instruction *instruction){
  uint32_t rd = instruction->regd_idx();
  set_reg(rd, lo);
}

// MFHI rd
void Cpu::op_mfhi(Instruction *instruction){
  uint32_t rd = instruction->regd_idx();
  set_reg(rd, hi);
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

// SH rt,offset(rs)
void Cpu::op_sh(Instruction *instruction){
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
  store16(addr, (uint16_t)reg(rt));
}

// SB rt,offset(rs)
void Cpu::op_sb(Instruction *instruction){
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
  uint32_t val = reg(rt);
  store8(addr, (uint8_t)val);
}

// LW rt,offset(rs)
void Cpu::op_lw(Instruction *instruction){
  if(sr & 0x10000 != 0){
    // Cache is isolated, ignore load
    printf("Ignoring load while cache is isolated\n");
    return;
  }
  
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  int32_t imm = instruction->imm_se();

  uint32_t addr = reg(rs) + imm;
  uint32_t value = load32(addr);
  //set_reg(rt, addr);

  // put the load in the delay slot
  load[0] = rt;
  load[1] = value;
}

// LB rt,offset(rs)
void Cpu::op_lb(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  int32_t imm = instruction->imm_se();

  uint32_t addr = reg(rs) + imm;
  // cast i8 to force sign extension
  int8_t value = load8(addr);

  // put the load in the delay slot
  load[0] = rt;
  load[1] = (uint32_t)value;
}

// LBU rt,offset(rs)
void Cpu::op_lbu(Instruction *instruction){
  // get register indices
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  int32_t imm = instruction->imm_se();

  uint32_t addr = reg(rs) + imm;

  // Put the load in the delay slot
  load[0] = rt;
  load[1] = (uint32_t)load8(addr);
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
  int32_t rt = (int32_t)instruction->regt_idx();
  uint32_t rd = instruction->regd_idx();
  int32_t sa = instruction->shift();

  set_reg(rd, reg(rt) >> sa);
}

// SRAV rd,rt,rs
void Cpu::op_srav(Instruction *instruction){
  // get register indices
  int32_t rt = (int32_t)instruction->regt_idx();
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
  next_pc = pc;
}

// J target
void Cpu::op_j(Instruction *instruction){
  uint32_t target = instruction->imm_jump();

  next_pc = (reg_pc & 0xf0000000) | (target << 2);
}

// JAL target (jump and link)
void Cpu::op_jal(Instruction *instruction){
  uint32_t ra = reg_pc;

  // Store return address in $31 ($ra)
  set_reg(31, ra);
  op_j(instruction);
}

// JR rs
void Cpu::op_jr(Instruction *instruction){
  uint32_t rs = instruction->regs_idx();
  reg_pc = reg(rs);
}

// JALR rs (jump and link register)
void Cpu::op_jalr(Instruction *instruction){
  uint32_t rd = instruction->regd_idx();
  uint32_t rs = instruction->regs_idx();
  
  uint32_t ra = reg_pc;

  // store return address in 'rd'
  set_reg(rd, ra);
  reg_pc = reg(rs);
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

// BEQ rs,rt,offset
void Cpu::op_beq(Instruction *instruction){
  uint32_t rs = instruction->regs_idx();
  uint32_t rt = instruction->regt_idx();
  int32_t imm = instruction->imm_se();

  if(reg(rs) == reg(rt)){
    branch(imm);
  }
}

// BGTZ rs,offset
void Cpu::op_bgtz(Instruction *instruction){
  uint32_t rs = instruction->regs_idx();
  int32_t imm = instruction->imm_se();
  
  if((int32_t)reg(rs) > 0)
    branch(imm);
}

// BLEZ rs,offset
void Cpu::op_blez(Instruction *instruction){
  uint32_t rs = instruction->regs_idx();
  int32_t imm = instruction->imm_se();
  
  if((int32_t)reg(rs) <= 0)
    branch(imm);
}

// BGEZ rs,offset, BLTZ rs,offset, BGEZAL rs,offset, BLTZAL rs,offset
// Bits 16 and 20 are used to figure out which one to use (position of rt)
void Cpu::op_bxx(Instruction *instruction){
  uint32_t rs = instruction->regs_idx();
  int32_t imm = instruction->imm_se();

  uint32_t instr = instruction->instr;
  
  uint32_t is_bgez = (instr >> 16) & 1;
  uint32_t is_link = (instr >> 17) & 0xf == 8;

  int32_t v = (int32_t)reg(rs);

  // Test "less than zero"
  uint32_t test = (uint32_t)(v < 0);

  // If the test is "greater than or equal to zero" we need to negate the comparison above since ("a>=0" <=> "!(a<0)"). The xor takes care of that
  test ^= is_bgez;

  if(is_link){
    uint32_t ra = reg_pc;
    // Store return address in r31
    set_reg(31, ra);
  }
  if(test != 0){
    branch(imm);
  }
}

// SYSCALL
void Cpu::op_syscall(Instruction *instruction){
  exception(SysCall);
}

// MFC0 rt,rd
// Move from Coprocessor 0
void Cpu::op_mfc0(Instruction *instruction){
  uint32_t cpu_r = instruction->regt_idx();
  uint32_t cop_r = instruction->regd_idx(); // TODO: in rust it is regd_idx().0, what is that? (might not need to check it out after all)

  uint32_t v;

  switch(cop_r){
    case 12:
      v = sr;
      break;
    // Cause register
    case 13:
      v = cause;
      break;
    case 14:
      v = epc;
      break;
    default:
      printf("Unhandled read from cop0r %d\n", cop_r);
      exit(1);
  }

  load[0] = cpu_r;
  load[1] = v;
}

// MTC0 rt,rd
// Move to Coprocessor 0
void Cpu::op_mtc0(Instruction *instruction){
  // get register indices
  uint32_t cpu_r = instruction->regt_idx();
  uint32_t cop_r = instruction->regd_idx(); // TODO: in rust it is regd_idx().0, what is that? (might not need to check it out after all)

  uint32_t v = reg(cpu_r);

  switch(cop_r){
    // breakpoints registers
    // 3 BPC, 5 BDA, 6 unknown(useless), 7 DCIC, 9 BDAM, 11, BPCM
    case 3 | 5 | 6 | 7 | 9 | 11:
      if(v != 0){
        printf("Unhandled write to cop0_r %d\n", cop_r);
        exit(1);
      }
      break;
    // reg 12 is SR
    case 12:
      sr = v;
      break;
    // reg 13 cause register (read only data -> cause of the excepion)
    case 13:
      if(v != 0){
        printf("Unhandled write to CAUSE register\n");
        exit(1);
      }
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

