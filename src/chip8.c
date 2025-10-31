#include "../include/chip8.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void initialize_chip(Chip8 *chip) {
	memset(chip->memory, 0, MEMORY_SIZE);
	chip->stack = &chip->memory[MEMORY_SIZE-1];
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

void update_display(Chip8 *chip){
	system("clear");
	for (int i=0; i<DISPLAY_HEIGHT; i++) {
		for (int j=0; j<DISPLAY_WIDTH; j++) {
			printf(chip->display[i][j] ? "* " : "   ");
		}
		printf("\n");
	}

#ifdef DEBUG
	display_chip_state(chip);
#endif
}

#ifdef DEBUG
int display_chip_state(Chip8 *chip) {
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
}
#endif
