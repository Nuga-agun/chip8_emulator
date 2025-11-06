#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>

void clear(Chip8* chip);
void update(Chip8* chip);
void draw(uint8_t X, uint8_t Y, uint8_t height, Chip8* chip);
