#ifndef _CPU_H
#define _CPU_H

#include "ines.h"

#define CPU_P_C 1
#define CPU_P_Z 2
#define CPU_P_I 4
#define CPU_P_D 8
#define CPU_P_B 16
#define CPU_P_V 64
#define CPU_P_N 128

typedef struct {
  nes_rom_t *rom;

  unsigned char error;
  int interrupt;

  unsigned char A; // 8-bit accumulator register
  unsigned char X, Y; // 8-bit index registers
  unsigned char P; // processor status flag bits (6x)
  unsigned char SP; // 8-bit stack pointer
  unsigned short PC; // 16-bit program counter

  char ram[2048];
  const char *prg;
  const char *chr;

  unsigned char ppu_registers[8];
} cpu_state_t;

void cpu_reset(cpu_state_t *cpu);
int cpu_step(cpu_state_t *cpu);

#endif
