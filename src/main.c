#include "../include/chip8.h"
#include "../include/display.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#define NIBBLE_MASK_1 0b1111000000000000
#define NIBBLE_MASK_2 0b0000111100000000
#define NIBBLE_MASK_3 0b0000000011110000
#define NIBBLE_MASK_4 0b0000000000001111

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
		uint16_t instruction = chip.memory[chip.pc]*256 + chip.memory[chip.pc+1];
#ifdef DEBUG
		printf("%04X : %04X -> ", chip.pc, instruction);
#endif
		chip.pc += 2;

		if (instruction == 0x00E0) {
#ifdef DEBUG
			printf("Clear screen\n");
#endif
			clear(&chip);
			update(&chip);
			continue;
		}
		uint8_t register_index = 0;
		uint16_t value = 0;
		switch(instruction>>12) {
			case 0x1:
				chip.pc = instruction&(NIBBLE_MASK_2|NIBBLE_MASK_3|NIBBLE_MASK_4);
#ifdef DEBUG
				printf("Jump to %X\n", chip.pc);
#endif
				break;
			case 0x6:
				register_index = (instruction&NIBBLE_MASK_2)>>8;
				value = instruction&(NIBBLE_MASK_3|NIBBLE_MASK_4);
				chip.V[register_index] = value;
#ifdef DEBUG
				printf("set register V%X to %02X\n", register_index, value);
#endif
				break;
			case 0x7:
				value = instruction&(NIBBLE_MASK_3|NIBBLE_MASK_4);
				register_index = (instruction&NIBBLE_MASK_2)>>8;
				chip.V[register_index] += value;
#ifdef DEBUG
				printf("add %02X to register V%X\n", value, register_index);
				printf("	-> V%X is now %02X\n", register_index, chip.V[register_index]);
#endif
				break;
			case 0xA:
				value = instruction&(~NIBBLE_MASK_1);
				chip.I = value;
#ifdef DEBUG
				printf("set index register to %03X\n", value);
#endif
				break;
			case 0xD:
#ifdef DEBUG
				printf("Draw !\n");
#endif
				break;
			default:
#ifdef DEBUG
				printf("\n");
#endif
		}
		if (instruction == 0) {
			return 0;
		}
		usleep(REFRESH_DELAY);
	} while (return_code == 0);
}
