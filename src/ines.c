#include <stdio.h>
#include <stdlib.h>

#include "ines.h"

#define INES_MAGIC 0x1a53454e
#define INES_F6_VRAM_MODE (1 | 8)
#define INES_F6_HAS_BATTERY_RAM 2
#define INES_F6_HAS_TRAINER 4
#define INES_F7_VS_UNISYSTEM 1
#define INES_F7_PLAYCHOICE_10 2
#define INES_F7_NES20 (4 | 8)
#define INES_F7_MAPPER_UPPER (16 | 32 | 64 | 128)
#define INES_F9_TV_SYSTEM 1
#define INES_F10_TV_SYSTEM (1 | 2)
#define INES_F10_PRG_RAM 16
#define INES_F10_BUS_CONFLICTS 32

nes_rom_t *open_nes_rom(const char *filename)
{
  printf("Opening ROM %s\n", filename);
  FILE *fp = fopen(filename, "rb");

  // Read header and verify magic bytes
  ines_header_t header;
  fread(&header, sizeof (ines_header_t), 1, fp);
  if (header.magic != INES_MAGIC) {
    fprintf(stderr, "Error: not a valid iNES ROM file\n");
    fclose(fp);
    return 0;
  }

  nes_rom_t *rom = (nes_rom_t *)malloc(sizeof (nes_rom_t));

  if (header.f6 & INES_F6_HAS_TRAINER) {
    fread(0, 512, 1, fp);
  }

  rom->prg_rom_size = 16384 * header.prg_rom_16kb;
  rom->prg_rom = (char *)malloc(rom->prg_rom_size);
  fread(rom->prg_rom, rom->prg_rom_size, 1, fp);

  rom->chr_rom_size = 8192 * header.chr_rom_8kb;
  rom->chr_rom = (char *)malloc(rom->chr_rom_size);
  fread(rom->chr_rom, rom->chr_rom_size, 1, fp);

  if (header.f7 & INES_F7_PLAYCHOICE_10) {
    fread(0, 8192, 1, fp);
  }

  rom->mapper_id = (header.f7 >> 4) | header.f6;

  fclose(fp);
  return rom;
}
