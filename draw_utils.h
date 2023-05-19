#ifndef DRAW_UTILS_H
#define DRAW_UTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"
#include "color_utils.h"

void draw_pixel(unsigned short *fb,int x, int y, unsigned short color);
void draw_pixel_big(unsigned short *fb,int x, int y, unsigned short color,int scale);
void draw_rectangle(unsigned short *fb,int x,int y,int w,int h,unsigned short color);
void connect_dots(unsigned short *fb,int x,int y,int pr_x,int pr_y,int w,int h,unsigned short color);
void fill_buffer(unsigned short *fb,unsigned int background_color);
void update_canvas(unsigned short *fb,void *parlcd_mem_base);
void draw_string(unsigned short *fb,int x, int y, char* string, unsigned short color,font_descriptor_t *fdes,int scale);
int char_width(font_descriptor_t *fdes,int ch);
void draw_char(unsigned short *fb,int x, int y, char ch, unsigned short color,font_descriptor_t *fdes,int scale);

#endif /*DRAW_UTILS_H*/