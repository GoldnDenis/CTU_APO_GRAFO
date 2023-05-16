#ifndef DRAW_UTILLS_H
#define DRAW_UTILLS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <termios.h> 

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

void draw_pixel(unsigned short *fb,int x, int y, unsigned short color);
void clear_buffer(unsigned short *fb,unsigned int background_color);
void update_canvas(unsigned short *fb,void *parlcd_mem_base);

void change_color_LED(unsigned int *color, uint8_t r, uint8_t g, uint8_t b);
void change_color_LCD(unsigned short *color, uint8_t r, uint8_t g, uint8_t b);
void change_RGB_lights(unsigned char *mem_base, unsigned int clr);

void change_tmp(unsigned char *mem_base, unsigned short r, unsigned short g, unsigned short b);
void set_brush_color(unsigned char *mem_base,unsigned short* clr,int* delta_knobs);
void set_background_color(unsigned short* background_color,unsigned short *fb,void *parlcd_mem_base);


#endif /*DRAW_UTILLS_H*/