#include "../include/chip8.h"

#include <stdio.h>
#include <stdlib.h>

int main () {
	Chip8 chip;
	initialize_chip(&chip);
	update_display(&chip);
}
