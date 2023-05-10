/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by 

  main.c - main file

  include your name there and license for distribution.

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L
 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
 
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
 
unsigned short *fb;

//  ((r>>3)<<11)|((g>>2)<<5)|(b>>3) - parse1 color

void draw_pixel(int x, int y, unsigned short color) {
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = color;
  }
  return;
}

void clear_buffer(unsigned short *fb){
  for (int ptr = 0; ptr < 320*480 ; ptr++) 
    fb[ptr]=0u;
  return;
}

void update_canvas(unsigned short *fb,void *parlcd_mem_base){
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (int ptr = 0; ptr < 480*320 ; ptr++) {
      parlcd_write_data(parlcd_mem_base, fb[ptr]);
  }
  return;
}

void change_color(unsigned short *color,unsigned short val){
  *color = val;
  return;
}
 
int main(int argc, char *argv[]) {
  unsigned char *parlcd_mem_base, *mem_base;
  int ptr;
  unsigned short clr=0x7ff;
  unsigned short background_clr=0;
  unsigned int c;
  fb  = (unsigned short *)malloc(320*480*2);
 
  printf("Hello world\n");
 
  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
  if (parlcd_mem_base == NULL)
    exit(1);
 
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
  if (mem_base == NULL)
    exit(1);
 
  parlcd_hx8357_init(parlcd_mem_base);
 
  clear_buffer(fb);
  update_canvas(fb,parlcd_mem_base);
  
  struct timespec loop_delay;
  loop_delay.tv_sec = 0;
  loop_delay.tv_nsec = 1 * 1000 * 1000;
  int xx=0, yy=0;
  while (1) {
 
    int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    if ((knobs&0x07000000)==0x07000000) {
      //Turn off if all pressed
      printf("knobs: %x\n",(knobs&0x07000000));
      break;
    }
    if ((knobs&0x07000000)==0x02000000) {
      //change color if G pressed
      printf("knobs: %x\n",(knobs&0x07000000));
      change_color(&clr,0xffff);
    }
    if ((knobs&0x07000000)==0x05000000) {
      // Clear canvas if R&B pressed
      printf("knobs: %x\n",(knobs&0x07000000));
      clear_buffer(fb);
      update_canvas(fb,parlcd_mem_base);
    }
    xx = ((knobs&0xff)*480)/256;
    yy = (((knobs>>16)&0xff)*320)/256;

    for (int j=0; j<5; j++)
      for (int i=0; i<5; i++) 
        draw_pixel(i+xx,j+yy,clr);
    update_canvas(fb,parlcd_mem_base);
 
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
  }
 
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (ptr = 0; ptr < 480*320 ; ptr++) {
    parlcd_write_data(parlcd_mem_base, 0);
  }
 
  printf("Goodbye world\n");
 
  return 0;
}