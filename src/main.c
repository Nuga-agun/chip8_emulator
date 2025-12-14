#include "../include/chip8.h"
#include "../include/display.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <pthread.h>

#define NIBBLE_MASK_1 0b1111000000000000
#define NIBBLE_MASK_2 0b0000111100000000
#define NIBBLE_MASK_3 0b0000000011110000
#define NIBBLE_MASK_4 0b0000000000001111

struct termios oldt;

void restore_terminal_settings() {
	tcsetattr(0, TCSANOW, &oldt);
}

void disable_waiting_for_enter()
{
     struct termios newt;
     tcgetattr(0, &oldt);
     newt = oldt;
     newt.c_lflag &= ~(ICANON | ECHO);
     tcsetattr(0, TCSANOW, &newt);
     atexit(restore_terminal_settings);
}

void* read_input(void* arg) {
	disable_waiting_for_enter();
	Chip8 chip = * (Chip8*) arg;
	while(1) {
		char input = getchar();
		keyboard_input(input, &chip);
	}
}

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

	pthread_t thread_input;
	pthread_create (&thread_input, NULL, read_input, &chip);
	int return_code = 0;
	do {
		uint16_t instruction = chip.memory[chip.pc]*256 + chip.memory[chip.pc+1];
		chip.pc += 2;

		uint8_t register_index = 0;
		uint8_t register_index_2 = 0;
		uint16_t value = 0;
		switch(instruction>>12) {
			case 0x0:
				if (instruction == 0x00E0) {
					clear(&chip);
					update(&chip);
#ifdef DEBUG
					printf("Clear screen\n");
#endif
				} else if (instruction = 0X00EE) {
					chip.pc = pop_from_stack(&chip);
#ifdef DEBUG
					printf("Return from subroutine\n");
#endif
				}
				break;
			case 0x1:
				chip.pc = instruction&(~NIBBLE_MASK_1);
#ifdef DEBUG
				printf("Jump to %00X\n", chip.pc);
#endif
				break;
			case 0x2:
				push_to_stack(chip.pc, &chip);
				value = instruction&(~NIBBLE_MASK_1);
				chip.pc = value;
#ifdef DEBUG
				printf("Subroutine at %02X\n", value);
#endif
				break;
			case 0x3:
				register_index = (instruction&NIBBLE_MASK_2)>>8;
				value = instruction&(NIBBLE_MASK_3|NIBBLE_MASK_4);
				if (value == chip.V[register_index]) {
					chip.pc += 2;
				}
#ifdef DEBUG
				printf("If V%X = %02X, skip next instruction", register_index, value);
#endif
				break;
			case 0x4:
				register_index = (instruction&NIBBLE_MASK_2)>>8;
				value = instruction&(NIBBLE_MASK_3|NIBBLE_MASK_4);
				if (value != chip.V[register_index]) {
					chip.pc += 2;
				}
#ifdef DEBUG
				printf("If V%X != %02X, skip next instruction", register_index, value);
#endif
				break;
			case 0x5:
				register_index = (instruction&NIBBLE_MASK_2)>>8;
				register_index_2 = (instruction&NIBBLE_MASK_3)>>4;
				if (chip.V[register_index] == chip.V[register_index_2]) {
					chip.pc += 2;
				}
#ifdef DEBUG
				printf("If V%X = V%X, skip next instruction", register_index, register_index_2);
#endif
				break;
			case 0x6:
				register_index = (instruction&NIBBLE_MASK_2)>>8;
				value = instruction&(NIBBLE_MASK_3|NIBBLE_MASK_4);
				chip.V[register_index] = value;
#ifdef DEBUG
				printf("Set register V%X = %02X\n", register_index, value);
#endif
				break;
			case 0x7:
				value = instruction&(NIBBLE_MASK_3|NIBBLE_MASK_4);
				register_index = (instruction&NIBBLE_MASK_2)>>8;
				chip.V[register_index] += value;
#ifdef DEBUG
				printf("Add value %d to register V%X\n", value, register_index);
#endif
				break;
			case 0x8:
				register_index = (instruction&NIBBLE_MASK_2)>>8;
				register_index_2 = (instruction&NIBBLE_MASK_3)>>4;
				value = (instruction&NIBBLE_MASK_4);
				uint8_t previous_VX = chip.V[register_index];
				switch(value) {
					case 0x0:
						chip.V[register_index] = chip.V[register_index_2];
#ifdef DEBUG
						printf("Set V%X to V%X\n", register_index, register_index_2);
#endif
						break;
					case 0x1:
						chip.V[register_index] = previous_VX | chip.V[register_index_2];
#ifdef DEBUG
						printf("Set V%X to (V%X OR V%X)\n", register_index, register_index, register_index_2);
#endif
						break;
					case 0x2:
						chip.V[register_index] = previous_VX & chip.V[register_index_2];
#ifdef DEBUG
						printf("Set V%X to (V%X AND V%X)\n", register_index, register_index, register_index_2);
#endif
						break;
					case 0x3:
						chip.V[register_index] = previous_VX ^ chip.V[register_index_2];
#ifdef DEBUG
						printf("Set V%X to (V%X XOR V%X)\n", register_index, register_index, register_index_2);
#endif
						break;
					case 0x4:
						chip.V[register_index] = previous_VX + chip.V[register_index_2];
#ifdef DEBUG
						printf("Set V%X to (V%X + V%X)\n", register_index, register_index, register_index_2);
#endif
						break;
					case 0x5:
						chip.V[register_index] = previous_VX - chip.V[register_index_2];
						chip.V[0xF] = chip.V[register_index_2] <= previous_VX ? 1 : 0;
#ifdef DEBUG
						printf("Set V%X to (V%X - V%X)\n", register_index, register_index, register_index_2);
#endif
						break;
					case 0x6:
#ifndef SUPER_CHIP
						chip.V[register_index] = chip.V[register_index_2];
						printf("Set V%X to V%X, then\n");
#endif
						chip.V[register_index] = previous_VX>>1;
#ifdef DEBUG
						printf("Shift the value of V%X 1 bit to the right\n", register_index);
#endif
						break;
					case 0x7:
						chip.V[register_index] = chip.V[register_index_2] - previous_VX;
						chip.V[0xF] = chip.V[register_index_2] >= previous_VX ? 1 : 0;
#ifdef DEBUG
						printf("Set V%X to (V%X - V%X)\n", register_index, register_index_2, register_index);
#endif
						break;
					case 0xE:
#ifndef SUPER_CHIP
						chip.V[register_index] = chip.V[register_index_2];
						printf("Set V%X to V%X, then\n");
#endif
						chip.V[register_index] = previous_VX<<1;
						chip.V[0xF] = previous_VX>>7;
#ifdef DEBUG
						printf("Shift the value of V%X 1 bit to the left\n", register_index);
#endif
						break;
					default:
						break;
				}
			case 0x9:
				register_index = (instruction&NIBBLE_MASK_2)>>8;
				register_index_2 = (instruction&NIBBLE_MASK_3)>>4;
				if (chip.V[register_index] != chip.V[register_index_2]) {
					chip.pc += 2;
				}
#ifdef DEBUG
				printf("If V%X != V%X, skip next instruction", register_index, register_index_2);
#endif
				break;
			case 0xA:
				value = instruction&(~NIBBLE_MASK_1);
				chip.I = value;
#ifdef DEBUG
				printf("Set index to %00X\n", value);
#endif
				break;
			case 0xB:
#ifdef SUPER_CHIP
				register_index = (instruction&NIBBLE_MASK_2)>>8;
#else
				register_index = 0;
#endif
				uint16_t address = instruction&(~NIBBLE_MASK_1) + chip.V[register_index];
				push_to_stack(chip.pc, &chip);
				chip.pc = address;
#ifdef DEBUG
				printf("Jump to %03X\n", address);
#endif
			case 0xC:
				register_index = (instruction&NIBBLE_MASK_2)>>8;
				value = instruction&(NIBBLE_MASK_3|NIBBLE_MASK_4);
				chip.V[register_index] = rand()&value;
#ifdef DEBUG
				printf("Set the result from (%0X AND *a random value*) to V%X", register_index);
#endif
			case 0xD:
				register_index = (instruction&NIBBLE_MASK_2)>>8;
				register_index_2 = (instruction&NIBBLE_MASK_3)>>4;
				uint8_t X = (chip.V[register_index])%DISPLAY_WIDTH;
				uint8_t Y = (chip.V[register_index_2])%DISPLAY_HEIGHT;
				uint8_t height = instruction&NIBBLE_MASK_4;
#ifdef DEBUG
				printf("Draw !\n\tCoordinates : (%02d,%02d); height = %d\n", X, Y, height);
#endif
				draw(X, Y, height, &chip);
				update(&chip);
				break;
			case 0xE:
				bool isPressed;
				register_index = (instruction&NIBBLE_MASK_2)>>8;
				value = chip.V[register_index];
				switch(instruction&(NIBBLE_MASK_3|NIBBLE_MASK_4)) {
					case 0x9E:
						isPressed = true;
						break;
					case 0xA1:
						isPressed = false;
						break;
					default:
						printf("Bad instruction : %04X\n", instruction);
						return 1;
				}
				if (get_key_state(value, &chip) == isPressed) {
					chip.pc += 2;
				}

#ifdef DEBUG
				printf("Skip next instruction if key corresponding to V%X is %spressed\n", register_index, isPressed ? "" : "not ");
#endif
			default:
		}
		if (instruction == 0) {
			return 0;
		}
		usleep(REFRESH_DELAY);
	} while (return_code == 0);
}
