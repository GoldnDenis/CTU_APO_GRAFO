#ifndef DRAWING_H
#define DRAWING_H
#define BUFFERS_LEN 320*480*2

#include <stdint.h>

#include "mzapo_regs.h"
#include "draw_utils.h"
#include "menus.h"

void start_drawing(unsigned char *mem_base, void* parlcd_mem_base, unsigned short *fb);

#endif /*DRAWING_H*/