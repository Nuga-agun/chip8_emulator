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
#ifndef DEBUG
	system("clear");
	printf("DEBUG non défini");
	for (int i=0; i<DISPLAY_HEIGHT; i++) {
		for (int j=0; j<DISPLAY_WIDTH; j++) {
			printf(chip->display[i][j] ? " * " : "   ");
		}
		printf("\n");
	}
#endif
}
