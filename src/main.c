#include "../include/chip8.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

char getch(void) {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcgetattr()");
    old.c_lflag &= ~ICANON; // Disable line buffering
    old.c_lflag &= ~ECHO;   // Disable echo
    old.c_cc[VMIN] = 1;     // Wait for at least one character
    old.c_cc[VTIME] = 0;    // No timeout
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;  // Restore line buffering
    old.c_lflag |= ECHO;    // Restore echo
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
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
	display_chip_state(&chip, true);
#endif

	int return_code = 0;
	do {
		//fetch
		//decode
		//execute
		if (getch() == 27) {
			printf("Keyboard Interruption");
			return_code = 1;
		}
	} while (return_code == 0);
}
