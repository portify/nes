#ifndef _INES_H
#define _INES_H

#include <stddef.h>

typedef struct {
  unsigned char mapper_id;
  size_t prg_ram_size;
  size_t prg_rom_size;
  size_t chr_rom_size;
  char *prg_rom;
  char *chr_rom;
} nes_rom_t;

typedef struct {
  int magic;
  unsigned char prg_rom_16kb;
  unsigned char chr_rom_8kb;
  unsigned char f6, f7;
  unsigned char prg_ram_8kb;
  unsigned char f9, f10;
  char zero[5];
} ines_header_t;

nes_rom_t *open_nes_rom(const char *filename);

#endif
