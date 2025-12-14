#include "../include/chip8.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ROMS_LOCATION "roms/"

void initialize_chip(Chip8 *chip) {
	memset(chip->memory, 0, MEMORY_SIZE);
	chip->stack = 0;
	chip->pc = 0x200;
	chip->I = 0;
	memset(chip->V, 0, 16);
	for (int i=0; i<DISPLAY_HEIGHT; i++) {
		for (int j=0; j<DISPLAY_WIDTH; j++) {
			chip->display[i][j] = 0;
		}
	}
	memset(chip->keyboard, 0, 16);
	chip->delay_timer = 0;
	chip->sound_timer = 0;
	load_font(chip);

	/* Initialize keyboard map */
	chip->keyboard_map[0] = '1';
	chip->keyboard_map[1] = '2';
	chip->keyboard_map[2] = '3';
	chip->keyboard_map[3] = '4';
	chip->keyboard_map[4] = 'a';
	chip->keyboard_map[5] = 'z';
	chip->keyboard_map[6] = 'e';
	chip->keyboard_map[7] = 'r';
	chip->keyboard_map[8] = 'q';
	chip->keyboard_map[9] = 's';
	chip->keyboard_map[10] = 'd';
	chip->keyboard_map[11] = 'f';
	chip->keyboard_map[12] = 'w';
	chip->keyboard_map[13] = 'x';
	chip->keyboard_map[14] = 'c';
	chip->keyboard_map[15] = 'v';
}

void load_font(Chip8 *chip) {
	const uint8_t fontset[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	for (int i=0; i<80; i++) {
		chip->memory[i] = fontset[i];
	}
}

void push_to_stack(uint16_t value, Chip8 *chip) {
	if (chip->stack > 16) {
		return;
	}
	chip->stack += 1;
	chip->memory[MEMORY_SIZE-(chip->stack)*2] = value>>8;
	chip->memory[MEMORY_SIZE-(chip->stack)*2-1] = value&0xFF;
#ifdef DEBUG
	printf("value : %02X; stack : %X\n", value, chip->stack);
#endif
}

uint16_t pop_from_stack(Chip8 *chip) {
	if (chip->stack <= 0) {
		return chip->pc;
	}
	uint16_t value;
	value = (chip->memory[MEMORY_SIZE-(chip->stack)*2])*0x100;
	value += chip->memory[MEMORY_SIZE-(chip->stack)*2-1];
	chip->stack -= 1;
#ifdef DEBUG
	printf("Stack index : %X\n", chip->stack + 1);
	printf("Stack value : %02X\n", value);
#endif
	return value;
}

void update_display(Chip8 *chip){
	system("clear");
	for (int i=0; i<DISPLAY_HEIGHT; i++) {
		for (int j=0; j<DISPLAY_WIDTH; j++) {
			printf(chip->display[i][j] ? "* " : "   ");
		}
		printf("\n");
	}

#ifdef DEBUG
	display_chip_state(chip, false);
#endif
}

int load_rom(char *rom_name, int name_length, Chip8 *chip) {
	char path[sizeof(ROMS_LOCATION) + name_length];
	strcat(strcpy(path, ROMS_LOCATION), rom_name);
	FILE *rom = NULL;
	rom = fopen(path, "r");
#ifdef DEBUG
	printf("Ouverture de la rom %s\n", path);
#endif
	if (rom == NULL) {
		printf("Erreur lors de l'ouverture du fichier\n");
		return 1;
	}

	int addr=chip->pc;
	int c;
	while (c != EOF) {
		c = fgetc(rom);
		chip->memory[addr] = c;
		addr++;
	}
	return 0;
}

void keyboard_input(char input, Chip8 *chip) {
	for (int i=0; i<16; i++) {
		if (chip->keyboard_map[i] == input) {
			chip->keyboard[i] = true;
		} else {
			chip->keyboard[i] = false;
		}
	}
}

bool get_key_state(char key, Chip8 *chip) {
	for (int i=0; i<16; i++) {
		if (chip->keyboard_map[i] == key) {
			return chip->keyboard[i];
		}
	}
	return false;
}

#ifdef DEBUG
void display_chip_state(Chip8 *chip, bool display_memory) {
	printf("\n-----CHIP STATUS-----\n");
	printf("PC ......... %p\n", chip->pc);
	printf("I ......... %p\n", chip->I);
	printf("\nREGISTERS :\n");
	printf("V0 ......... %p\n", chip->V[0]);
	printf("V1 ......... %p\n", chip->V[1]);
	printf("V2 ......... %p\n", chip->V[2]);
	printf("V3 ......... %p\n", chip->V[3]);
	printf("V4 ......... %p\n", chip->V[4]);
	printf("V5 ......... %p\n", chip->V[5]);
	printf("V6 ......... %p\n", chip->V[6]);
	printf("V7 ......... %p\n", chip->V[7]);
	printf("V8 ......... %p\n", chip->V[8]);
	printf("V9 ......... %p\n", chip->V[9]);
	printf("V10 ........ %p\n", chip->V[10]);
	printf("V11 ........ %p\n", chip->V[11]);
	printf("V12 ........ %p\n", chip->V[12]);
	printf("V13 ........ %p\n", chip->V[13]);
	printf("V14 ........ %p\n", chip->V[14]);
	printf("V15 ........ %p\n", chip->V[15]);
	printf("\nTIMERS :\n");
	printf("delay....... %p\n", chip->delay_timer);
	printf("sound....... %p\n", chip->sound_timer);
	printf("\n");

	if (display_memory) {
		for (int i=0; i<MEMORY_SIZE; i++) {
			printf(i ==chip->pc ? " _ %02x _ ;" : " %02x ;", chip->memory[i]);
		}
	}
	printf("\n");
}
#endif
