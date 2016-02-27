#include <stdio.h>

#include "cpu.h"

// #define CPU_DEBUG_RAM
#define CPU_DEBUG_STEPS
#define CPU_DEBUG_OPS

enum {
  InterruptNone,
  InterruptNMI,
  InterruptReset,
  InterruptIRQ
};

enum {
	AddressInvalid,
	AddressAbsolute,
	AddressAbsoluteX,
	AddressAbsoluteY,
	AddressAccumulator,
	AddressImmediate,
	AddressImplied,
	AddressIndexedIndirect,
	AddressIndirect,
	AddressIndirectIndexed,
	AddressRelative,
	AddressZeroPage,
	AddressZeroPageX,
	AddressZeroPageY
};

#define OP_COUNT 75
enum {
  opBRK, opORA, opKIL, opSLO, opNOP, opASL, opPHP, opANC, opBPL, opCLC, opJSR,
  opAND, opRLA, opBIT, opROL, opPLP, opBMI, opSEC, opRTI, opEOR, opSRE, opLSR,
  opPHA, opALR, opJMP, opBVC, opCLI, opRTS, opADC, opRRA, opROR, opPLA, opARR,
  opBVS, opSEI, opSTA, opSAX, opSTY, opSTX, opDEY, opTXA, opXAA, opBCC, opAHX,
  opTYA, opTXS, opTAS, opSHY, opSHX, opLDY, opLDA, opLDX, opLAX, opTAY, opTAX,
  opBCS, opCLV, opTSX, opLAS, opCPY, opCMP, opDCP, opDEC, opINY, opDEX, opAXS,
  opBNE, opCLD, opCPX, opSBC, opISC, opINX, opBEQ, opSED, opINC
};
char *opName[OP_COUNT] = {
  "BRK", "ORA", "KIL", "SLO", "NOP", "ASL", "PHP", "ANC", "BPL", "CLC", "JSR",
  "AND", "RLA", "BIT", "ROL", "PLP", "BMI", "SEC", "RTI", "EOR", "SRE", "LSR",
  "PHA", "ALR", "JMP", "BVC", "CLI", "RTS", "ADC", "RRA", "ROR", "PLA", "ARR",
  "BVS", "SEI", "STA", "SAX", "STY", "STX", "DEY", "TXA", "XAA", "BCC", "AHX",
  "TYA", "TXS", "TAS", "SHY", "SHX", "LDY", "LDA", "LDX", "LAX", "TAY", "TAX",
  "BCS", "CLV", "TSX", "LAS", "CPY", "CMP", "DCP", "DEC", "INY", "DEX", "AXS",
  "BNE", "CLD", "CPX", "SBC", "ISC", "INX", "BEQ", "SED", "INC"
};

char instructionOp[256] = {
  opBRK, opORA, opKIL, opSLO, opNOP, opORA, opASL, opSLO,
  opPHP, opORA, opASL, opANC, opNOP, opORA, opASL, opSLO,
  opBPL, opORA, opKIL, opSLO, opNOP, opORA, opASL, opSLO,
  opCLC, opORA, opNOP, opSLO, opNOP, opORA, opASL, opSLO,
  opJSR, opAND, opKIL, opRLA, opBIT, opAND, opROL, opRLA,
  opPLP, opAND, opROL, opANC, opBIT, opAND, opROL, opRLA,
  opBMI, opAND, opKIL, opRLA, opNOP, opAND, opROL, opRLA,
  opSEC, opAND, opNOP, opRLA, opNOP, opAND, opROL, opRLA,
  opRTI, opEOR, opKIL, opSRE, opNOP, opEOR, opLSR, opSRE,
  opPHA, opEOR, opLSR, opALR, opJMP, opEOR, opLSR, opSRE,
  opBVC, opEOR, opKIL, opSRE, opNOP, opEOR, opLSR, opSRE,
  opCLI, opEOR, opNOP, opSRE, opNOP, opEOR, opLSR, opSRE,
  opRTS, opADC, opKIL, opRRA, opNOP, opADC, opROR, opRRA,
  opPLA, opADC, opROR, opARR, opJMP, opADC, opROR, opRRA,
  opBVS, opADC, opKIL, opRRA, opNOP, opADC, opROR, opRRA,
  opSEI, opADC, opNOP, opRRA, opNOP, opADC, opROR, opRRA,
  opNOP, opSTA, opNOP, opSAX, opSTY, opSTA, opSTX, opSAX,
  opDEY, opNOP, opTXA, opXAA, opSTY, opSTA, opSTX, opSAX,
  opBCC, opSTA, opKIL, opAHX, opSTY, opSTA, opSTX, opSAX,
  opTYA, opSTA, opTXS, opTAS, opSHY, opSTA, opSHX, opAHX,
  opLDY, opLDA, opLDX, opLAX, opLDY, opLDA, opLDX, opLAX,
  opTAY, opLDA, opTAX, opLAX, opLDY, opLDA, opLDX, opLAX,
  opBCS, opLDA, opKIL, opLAX, opLDY, opLDA, opLDX, opLAX,
  opCLV, opLDA, opTSX, opLAS, opLDY, opLDA, opLDX, opLAX,
  opCPY, opCMP, opNOP, opDCP, opCPY, opCMP, opDEC, opDCP,
  opINY, opCMP, opDEX, opAXS, opCPY, opCMP, opDEC, opDCP,
  opBNE, opCMP, opKIL, opDCP, opNOP, opCMP, opDEC, opDCP,
  opCLD, opCMP, opNOP, opDCP, opNOP, opCMP, opDEC, opDCP,
  opCPX, opSBC, opNOP, opISC, opCPX, opSBC, opINC, opISC,
  opINX, opSBC, opNOP, opSBC, opCPX, opSBC, opINC, opISC,
  opBEQ, opSBC, opKIL, opISC, opNOP, opSBC, opINC, opISC,
  opSED, opSBC, opNOP, opISC, opNOP, opSBC, opINC, opISC
};

char instructionAddressMode[256] = {
	6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
	1, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
	6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
	6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 8, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
	5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
	5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
	5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
	5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
};

unsigned char mr(cpu_state_t *cpu, unsigned short addr)
{
  if (addr < 0x2000) {
    return cpu->ram[addr & 0x800];
  } else if (addr < 0x4000) {
    printf("mr: read ppu register %d\n", addr & 0x8);
    return cpu->ppu_registers[addr & 0x8];
  } else if (addr < 0x4020) {
    // NES APU and I/O registers
  } else {
    // Cartridge space
    // Call out to mapper here
    // Assuming mapper ID 0
    if (addr >= 0x8000 && addr < 0xc000) {
      return cpu->rom->prg_rom[addr - 0x8000];
    } else if (addr >= 0xc000) {
      return cpu->rom->prg_rom[(cpu->rom->prg_rom_size - 16384) + (addr - 0xc000)];
    }
  }

  fprintf(stderr, "mr: %x failed\n", addr);
  return 0;
}

#ifdef CPU_DEBUG_RAM
unsigned char mr_debug(cpu_state_t *cpu, unsigned short addr)
{
  unsigned char data = mr(cpu, addr);
  printf("mem read  %x   (%x)\n", addr, data);
  return data;
}
#define mr mr_debug
#endif

unsigned char mw(cpu_state_t *cpu, unsigned short addr, unsigned char data)
{
#ifdef CPU_DEBUG_RAM
  printf("mem write %x := %x\n", addr, data);
#endif

  if (addr < 0x2000) {
    cpu->ram[addr & 0x800] = data;
  } else if (addr < 0x4000) {
    fprintf(stderr, "mw: ignoring write to ppu register %x\n", addr & 0x8);
  } else if (addr < 0x4020) {
    fprintf(stderr, "mw: ignoring write to apu/io %x\n", addr - 0x4000);
  } else {
    fprintf(stderr, "mw: illegal write to rom %x\n", addr);
  }

  return data;
}

unsigned short mr16(cpu_state_t *cpu, unsigned short addr)
{
  return (mr(cpu, addr + 1) << 8) | mr(cpu, addr);
}

unsigned short mw16(cpu_state_t *cpu, unsigned short addr, unsigned short data)
{
  mw(cpu, addr + 1, data >> 8);
  mw(cpu, addr, data & 0xff);
  return data;
}

unsigned short mr16bad(cpu_state_t *cpu, unsigned short addr)
{
  unsigned short next = (addr & 0xff00) | ((unsigned char)addr + 1);
  return (unsigned short)mr(cpu, next) << 8 | mr(cpu, addr);
}

void setz(cpu_state_t *cpu, unsigned char data)
{
  if (data == 0) {
    cpu->P |= CPU_P_Z;
  } else {
    cpu->P &= ~CPU_P_Z;
  }
}

void setn(cpu_state_t *cpu, unsigned char data)
{
  if (data & 0x80) {
    cpu->P |= CPU_P_N;
  } else {
    cpu->P &= ~CPU_P_N;
  }
}

void setzn(cpu_state_t *cpu, unsigned char data)
{
  setz(cpu, data);
  setn(cpu, data);
}

char ispagediff(unsigned short a, unsigned short b)
{
  return (a & 0xff00) != (b & 0xff00);
}

void cpu_jump(cpu_state_t *cpu, unsigned short addr)
{
  printf("jump to %x\n", addr);
  cpu->PC = addr;
}

void cpu_compare(cpu_state_t *cpu, unsigned char a, unsigned char b)
{
  setn(cpu, a - b);
  cpu->P &= ~CPU_P_C;
  if (a >= b)
    cpu->P |= CPU_P_C;
}

void cpu_push(cpu_state_t *cpu, unsigned char data)
{
  mw(cpu, 0x100 | cpu->SP--, data);
}

void cpu_push16(cpu_state_t *cpu, unsigned short data)
{
  cpu_push(cpu, data & 0xff);
  cpu_push(cpu, data >> 8);
}

unsigned char cpu_pull(cpu_state_t *cpu)
{
  return mr(cpu, 0x100 | ++cpu->SP);
}

unsigned short cpu_pull16(cpu_state_t *cpu)
{
  unsigned char hi = cpu_pull(cpu);
  unsigned char lo = cpu_pull(cpu);
  return (unsigned short)hi << 8 | lo;
}

void cpu_reset(cpu_state_t *cpu)
{
  printf("======== reset\n");

  cpu->error = 0;
  cpu->interrupt = InterruptReset;

  cpu->P = 0x34;
  cpu->SP = 0xFD;

  for (size_t i = 0; i < 8; i++)
    cpu->ppu_registers[i] = 255;
}

int cpu_step(cpu_state_t *cpu)
{
  if (cpu->interrupt != InterruptNone) {
    cpu_push16(cpu, cpu->PC);
    cpu_push(cpu, cpu->P);

    switch (cpu->interrupt) {
    case InterruptIRQ: cpu->PC = mr16(cpu, 0xfffe); cpu->P |= CPU_P_B; break;
    case InterruptReset: cpu->PC = mr16(cpu, 0xfffc); break;
    case InterruptNMI: cpu->PC = mr16(cpu, 0xfffa); break;
    }

    printf("interrupt to %x\n", cpu->PC);
    cpu->interrupt = InterruptNone;
    cpu->P |= CPU_P_I;
  }

  if (cpu->PC < 0x4020) {
    fprintf(stderr, "bad: PC outside cartridge space (%x)!\n", cpu->PC);
    cpu->error = 1;
    return 0;
  }

#ifdef CPU_DEBUG_STEPS
  printf("======== step A:%x X:%x Y:%x PC:%x\n", cpu->A, cpu->X, cpu->Y, cpu->PC);
#endif

  unsigned short ipc = cpu->PC;
  unsigned char ins = mr(cpu, cpu->PC++);
  int op = instructionOp[ins];
  int mode = instructionAddressMode[ins];

#ifdef CPU_DEBUG_OPS
  printf("[%x] %s (%x)\n", ipc, opName[op], ins);
#endif

  unsigned short address;

  switch (mode) {
  case AddressAbsolute:
    address = mr16(cpu, cpu->PC);
    cpu->PC += 2;
    break;
  case AddressAbsoluteX:
    address = mr16(cpu, cpu->PC + 1) + cpu->X;
    break;
  case AddressAbsoluteY:
    address = mr16(cpu, cpu->PC + 1) + cpu->Y;
    break;
  case AddressImmediate:
    address = cpu->PC++;
    break;
  case AddressAccumulator:
  case AddressImplied:
    address = 0;
    break;
  case AddressIndexedIndirect:
    address = mr16bad(cpu, (unsigned short)mr(cpu, cpu->PC++) + cpu->X);
    break;
  case AddressIndirect:
    address = mr16bad(cpu, mr16(cpu, cpu->PC++));
    break;
  case AddressIndirectIndexed:
    address = mr16bad(cpu, mr(cpu, cpu->PC++)) + cpu->Y; // ??
    break;
  case AddressRelative:
  {
    unsigned short offset = mr(cpu, cpu->PC);
    cpu->PC += 1;
    if (offset < 0x80) {
      address = cpu->PC + offset;
    } else {
      address = cpu->PC + offset - 0x100;
    }
    break;
  }
  case AddressZeroPage:
    address = mr(cpu, cpu->PC++);
    break;
  case AddressZeroPageX:
    address = (unsigned short)mr(cpu, cpu->PC++) + cpu->X;
    break;
  case AddressZeroPageY:
    address = (unsigned short)mr(cpu, cpu->PC++) + cpu->Y;
    break;
  default:
    fprintf(stderr, "unknown address mode %x for %x\n", mode, ins);
    cpu->error = 1;
    return 0;
  }

  switch (op) {
  case opADC:
  {
    unsigned char a = cpu->A;
  	unsigned char b = mr(cpu, address);
  	unsigned char c = cpu->P & CPU_P_C;
  	setzn(cpu, cpu->A = a + b + c);
    if ((int)a + (int)b + (int)c > 0xff) {
      cpu->P |= CPU_P_C;
    } else {
      cpu->P &= ~CPU_P_C;
    }
  	if (((a^b) & 0x80) == 0 && ((a ^ cpu->A) & 0x80) != 0) {
  		cpu->P |= CPU_P_V;
  	} else {
  		cpu->P &= ~CPU_P_V;
  	}
    return 0;
  }
  case opAND:
    setzn(cpu, cpu->A &= mr(cpu, address));
    return 0;
  // TODO: opASL
  case opBCC:
    if (!(cpu->P & CPU_P_C)) cpu_jump(cpu, address);
    return 0;
  case opBCS:
    if (cpu->P & CPU_P_C) cpu_jump(cpu, address);
    return 0;
  case opBEQ:
    if (cpu->P & CPU_P_Z) cpu_jump(cpu, address);
    return 0;
  case opBIT:
  {
    unsigned char value = mr(cpu, address);
    if ((value >> 6) & 1) {
      cpu->P |= CPU_P_V;
    } else {
      cpu->P &= ~CPU_P_V;
    }
    setz(cpu, value & cpu->A);
    setn(cpu, value);
    return 0;
  }
  case opBMI:
    if (cpu->P & CPU_P_N) cpu_jump(cpu, address);
    return 0;
  case opBNE:
    if (!(cpu->P & CPU_P_Z)) cpu_jump(cpu, address);
    return 0;
  case opBPL:
    if (!(cpu->P & CPU_P_N)) cpu_jump(cpu, address);
    return 0;
  case opBRK:
    cpu->interrupt = InterruptIRQ;
    return 0;
  case opBVC:
    if (!(cpu->P & CPU_P_V)) cpu_jump(cpu, address);
    return 0;
  case opBVS:
    if (cpu->P & CPU_P_V) cpu_jump(cpu, address);
    return 0;
  case opCLC:
    cpu->P &= ~CPU_P_C;
    return 0;
  case opCLD:
    cpu->P &= ~CPU_P_D;
    return 0;
  case opCLI:
    cpu->P &= ~CPU_P_I;
    return 0;
  case opCLV:
    cpu->P &= ~CPU_P_V;
    return 0;
  case opCMP:
    cpu_compare(cpu, cpu->A, mr(cpu, address));
    return 0;
  case opCPX:
    cpu_compare(cpu, cpu->X, mr(cpu, address));
    return 0;
  case opCPY:
    cpu_compare(cpu, cpu->Y, mr(cpu, address));
    return 0;
  case opDEC:
    setzn(cpu, mw(cpu, address, mr(cpu, address) - 1));
    return 0;
  case opDEX:
    setzn(cpu, --cpu->X);
    return 0;
  case opDEY:
    setzn(cpu, --cpu->Y);
    return 0;
  case opEOR:
    setzn(cpu, cpu->A ^= mr(cpu, address));
    return 0;
  case opINC:
    setzn(cpu, mw(cpu, address, mr(cpu, address) + 1));
    return 0;
  case opINX:
    setzn(cpu, ++cpu->X);
    return 0;
  case opINY:
    setzn(cpu, ++cpu->Y);
    return 0;
  case opJMP:
    cpu_jump(cpu, address);
    return 0;
  case opJSR:
    cpu_push16(cpu, cpu->PC - 1);
    cpu_jump(cpu, address);
    return 0;
  case opLDA:
    setzn(cpu, cpu->A = mr(cpu, address));
    return 0;
  case opLDX:
    setzn(cpu, cpu->X = mr(cpu, address));
    return 0;
  case opLDY:
    setzn(cpu, cpu->Y = mr(cpu, address));
    return 0;
  case opLSR:
    if (mode == AddressAccumulator) {
      if (cpu->A & 1) {
        cpu->P |= CPU_P_C;
      } else {
        cpu->P &= ~CPU_P_C;
      }
      setzn(cpu, cpu->A >>= 1);
    } else {
      unsigned char data = mr(cpu, address);
      if (data & 1) {
        cpu->P |= CPU_P_C;
      } else {
        cpu->P &= ~CPU_P_C;
      }
      data >>= 1;
      setzn(cpu, mw(cpu, address, data));
    }
    return 0;
  case opNOP:
    return 0;
  case opORA:
    setzn(cpu, cpu->A |= mr(cpu, address));
    return 0;
  case opPHA:
    cpu_push(cpu, cpu->A);
    return 3;
  case opPHP:
    cpu_push(cpu, cpu->P);
    return 3;
  case opPLA:
    setzn(cpu, cpu->A = cpu_pull(cpu));
    return 4;
  case opPLP:
    cpu->P = cpu_pull(cpu);
    return 4;
  // TODO: opROL
  // TODO: opROR
  case opRTI:
    cpu->P = cpu_pull(cpu);
    // return 0; // fall through
  case opRTS:
    cpu_jump(cpu, cpu_pull16(cpu) + 1);
    return 0;
  // TODO: opSBC
  case opSEC:
    cpu->P |= CPU_P_C;
    return 0;
  case opSED:
    cpu->P |= CPU_P_D;
    return 0;
  case opSEI:
    cpu->P |= CPU_P_I;
    return 0;
  case opSTA:
    mw(cpu, address, cpu->A);
    return 0;
  case opSTX:
    mw(cpu, address, cpu->X);
    return 0;
  case opSTY:
    mw(cpu, address, cpu->Y);
    return 0;
  case opTAX:
    setzn(cpu, cpu->X = cpu->A);
    return 2;
  case opTAY:
    setzn(cpu, cpu->Y = cpu->A);
    return 2;
  case opTSX:
    setzn(cpu, cpu->X = cpu->SP);
    return 2;
  case opTXA:
    setzn(cpu, cpu->A = cpu->X);
    return 2;
  case opTXS:
    setzn(cpu, cpu->SP = cpu->X);
    return 2;
  case opTYA:
    setzn(cpu, cpu->A = cpu->Y);
    return 2;

  default:
    cpu->error = 1;
    fprintf(stderr, "unhandled instruction: %x %s\n", ins, opName[op]);
    return 0;
  }
}
