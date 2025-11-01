#include "../include/chip8.h"

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[]) {
	if (argc != 2) {
		printf("Utilisation : chip <ROM_NAME>");
		return 1;
	}

	Chip8 chip;
	initialize_chip(&chip);

	if (load_rom(argv[1], sizeof(argv[1]), &chip) != 0) {
		printf("Une erreur est survenue au chargement de la rom\n");
		return 1;
	}
#ifdef DEBUG
	display_chip_state(&chip, true);
#endif

	int return_code = 0;
	do {
		//fetch
		//decode
		//execute
		return_code = 1;
	} while (return_code == 0);
}
