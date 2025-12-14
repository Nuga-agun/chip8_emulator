#include "../include/chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void clear(Chip8 *chip) {
	for (int i=0; i<DISPLAY_HEIGHT; i++) {
		for (int j=0; j<DISPLAY_WIDTH; j++) {
			chip->display[i][j] = false;
		}
	}
}

void update(Chip8 *chip) {
	printf("\n\n+");
	for (int j=0; j<DISPLAY_WIDTH; j++) {
		printf("--");
	}
	printf("+\n");
	for (int i=0; i<DISPLAY_HEIGHT; i++) {
		printf("|");
		for (int j=0; j<DISPLAY_WIDTH; j++) {
			printf(chip->display[i][j] ? "||" : "  ");
		}
		printf("|\n");
	}
	printf("+");
	for (int j=0; j<DISPLAY_WIDTH; j++) {
		printf("--");
	}
	printf("+\n");
}

void draw(uint8_t X, uint8_t Y, uint8_t height, Chip8* chip) {
	chip->V[15] = 0;
	for (int i=0; i<height; i++) {
		uint8_t sprite = chip->memory[(chip->I) + i];
		if (i+Y >= DISPLAY_HEIGHT) {
			break;
		}
		for (int j=0; j<8; j++) {
			if (((sprite>>(7-j))&0b1) != 0) {
				if (chip->display[i+Y][j+X] == 1) {
					chip->V[16] = 1;
				}
				chip->display[i+Y][j+X] = !chip->display[i+Y][j+X]; 
			}
		}
	}
}
