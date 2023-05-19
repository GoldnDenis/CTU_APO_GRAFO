#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include <stdint.h>

#include "mzapo_regs.h"

void unite_RGB(unsigned int *color, uint8_t r, uint8_t g, uint8_t b);
void convert_RGB_to_LCD(unsigned short *color, uint8_t r, uint8_t g, uint8_t b);
void display_LED_lights(unsigned char *mem_base, unsigned short r, unsigned short g, unsigned short b);

#endif /*COLOR_UTILS_H*/