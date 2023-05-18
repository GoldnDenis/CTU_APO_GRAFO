#ifndef DRAW_UTILLS_H
#define DRAW_UTILLS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"

enum Program_state {
    START,
    TUTORIAL,
    EXIT
};

void draw_pixel(unsigned short *fb,int x, int y, unsigned short color);
void draw_rectangle(unsigned short *fb,int x,int y,int w,int h,unsigned short color);
void clear_buffer(unsigned short *fb,unsigned int background_color);
void update_canvas(unsigned short *fb,void *parlcd_mem_base);

void change_color_LED(unsigned int *color, uint8_t r, uint8_t g, uint8_t b);
void change_color_LCD(unsigned short *color, uint8_t r, uint8_t g, uint8_t b);
void change_RGB_lights(unsigned char *mem_base, unsigned int clr);

void draw_main_menu(unsigned char *mem_base, void* parlcd_mem_base, unsigned short *fb, unsigned short* background_color);
void change_tmp(unsigned char *mem_base, unsigned short r, unsigned short g, unsigned short b);
void set_background_color(unsigned char *mem_base,void *parlcd_mem_base,unsigned short *fb,unsigned short* background_clr);
void draw_pixel_big(unsigned short *fb,int x, int y, unsigned short color,int scale);
int char_width(font_descriptor_t *fdes,int ch);
void draw_char(unsigned short *fb,int x, int y, char ch, unsigned short color,font_descriptor_t *fdes,int scale);
void set_brush_color(unsigned char *mem_base,void *parlcd_mem_base,unsigned short *fb,unsigned short* clr,short* brush_size,int* delta_knobs);


#endif /*DRAW_UTILLS_H*/