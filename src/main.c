#include "../include/chip8.h"

#include <stdio.h>
#include <stdlib.h>

#define DEBUG

int main () {
#ifdef DEBUG
	printf("Lancement en mode DEBUG");
#endif
	Chip8 chip;
	initialize_chip(&chip);
}
