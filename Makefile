CC = gcc
CFLAGS = -std=c99 -Wall

LD = gcc -o
LFLAGS = -Wall

SOURCES = src/ines.c src/cpu.c src/main.c
OBJECTS = $(SOURCES:src/%.c=obj/%.o)

all: bin/nes
#	bin/nes

# bin/nes:
# 	gcc -o bin/nes src/main.c

bin/nes: $(OBJECTS)
	$(LD) $@ $(LFLAGS) $(OBJECTS)

$(OBJECTS): obj/%.o : src/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
