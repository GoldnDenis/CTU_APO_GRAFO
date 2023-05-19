#ifndef MENUS_H
#define MENUS_H
#define BUFFERS_LEN 320*480*2

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
#include "draw_utils.h"
#include "color_utils.h"
#include "timers.h"

enum Program_state {
    START,
    TUTORIAL,
    EXIT
};

void draw_main_menu(unsigned char *mem_base, void* parlcd_mem_base, unsigned short *fb);
void set_background_color(unsigned char *mem_base,void *parlcd_mem_base,unsigned short *fb,unsigned short* background_clr);
void set_brush_size(unsigned char *mem_base,void *parlcd_mem_base,unsigned short *fb,unsigned short* clr,short* brush_size,int* delta_knobs);
void set_brush_color(unsigned char *mem_base,void *parlcd_mem_base,unsigned short *fb,unsigned short* clr,short* brush_size,int* delta_knobs);
void start_drawing(unsigned char *mem_base, void* parlcd_mem_base, unsigned short *fb);

#endif /*MENUS_H*/