#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE 4096
#define DISPLAY_HEIGHT 32
#define DISPLAY_WIDTH 64
#define REFRESH_DELAY 100000

typedef struct {
	uint8_t memory[MEMORY_SIZE]; //RAM
	uint8_t stack; //Stack (intégrée à la mémoire)
	uint16_t pc; //Program counter
	uint16_t I; //Index register
	uint8_t V[16]; //Registres
	bool display[DISPLAY_HEIGHT][DISPLAY_WIDTH]; 
	bool keyboard[16];
	uint8_t delay_timer;
	uint8_t sound_timer;
} Chip8;

void initialize_chip(Chip8 *chip);
void load_font(Chip8 *chip);
void push_to_stack(uint16_t value, Chip8 *chip);
void pop_from_stack(Chip8 *chip);
void update_display(Chip8 *chip);
int load_rom(char *rom_name, int name_length, Chip8 *chip);
#ifdef DEBUG
void display_chip_state(Chip8 *chip, bool display_memory);
#endif

#endif
