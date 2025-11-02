#include "../include/chip8.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

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
	display_chip_state(&chip, false);
#endif

	int return_code = 0;
	do {
		//fetch
		uint16_t instruction = chip.memory[chip.pc]*256 + chip.memory[chip.pc+1];
		chip.pc += 2;
#ifdef DEBUG
		printf("%04X -> ", instruction, instruction>>12);
#endif

		//decode
		if (instruction == 0x00E0) {
			printf("Clear screen !\n");
		}
		switch(instruction>>12) {
			case 0x1:
				printf("Jump !\n");
				break;
			case 0x6:
				printf("set register V%X\n", (instruction>>8)&0b1111);
				break;
			case 0x7:
				printf("add value to register V%X\n", (instruction>>8)&0b1111);
				break;
			case 0xA:
				printf("set index register\n");
				break;
			case 0xD:
				printf("Draw !\n");
				break;
			default:
				printf("\n");
		}
		if (instruction == 0) {
			return 0;
		}
		//execute
	} while (return_code == 0);
}
