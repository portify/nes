#include <stdio.h>

#include "cpu.h"

int main(int argc, char *argv[])
{
  nes_rom_t *rom = open_nes_rom("roms/donkeykong.nes");

  if (rom == 0) {
    return 1;
  }

  nes_cpu_state cpu;
  cpu.rom = rom;
  nes_cpu_reset(&cpu);

  while (1) {
    unsigned short cpu_cycles = nes_cpu_step(&cpu);

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
  }

  return 0;
}
