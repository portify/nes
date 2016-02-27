#include <stdio.h>

#include "cpu.h"

int main(int argc, char *argv[])
{
  nes_rom_t *rom = open_nes_rom("roms/nestest.nes");

  if (rom == 0) {
    return 1;
  }

  cpu_state_t cpu;
  cpu.rom = rom;
  cpu.interrupt = 0;
  cpu.PC = 0xc000;
  cpu_reset(&cpu);
  // cpu_step(&cpu);
  // cpu_step(&cpu);
  // cpu_step(&cpu);
  // cpu_step(&cpu);
  // cpu_step(&cpu);

  while (1) {
    unsigned short cpu_cycles = cpu_step(&cpu);

    if (cpu.error)
      break;

    unsigned int ppu_cycles = cpu_cycles * 3;

    for (unsigned int i = 0; i < ppu_cycles; i++) {
      // nes_ppu_step
      // nes_mapper_step
    }

    for (unsigned short i = 0; i < cpu_cycles; i++) {
      // nes_apu_step
    }

    // if (getch() == 27)
    //   break;
  }

  return 0;
}
