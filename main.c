  /*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by 

  main.c - main file

  include your name there and license for distribution.

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L
 
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
 
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "draw_utills.h"

int main(int argc, char *argv[]) 
{

  int delta_knobs = 0;

  uint32_t val_line = 1;

  unsigned char *parlcd_mem_base, *mem_base;
  int ptr;
  unsigned short clr = 0xffff; // "WHITE"
  unsigned short background_clr = 0;
  unsigned int c;
  unsigned short *fb  = (unsigned short *)malloc(320*480*2);
 
  printf("Hello world\n");
 
  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
  if (parlcd_mem_base == NULL)
    exit(1);
 
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
  if (mem_base == NULL)
    exit(1);
 
  parlcd_hx8357_init(parlcd_mem_base);
 
  clear_buffer(fb,background_clr);
  update_canvas(fb,parlcd_mem_base);
  
  struct timespec loop_delay;
  loop_delay.tv_sec = 0;
  loop_delay.tv_nsec = 1 * 1000 * 1000;
  int xx=0, yy=0;
  int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  delta_knobs += knobs;
  set_background_color(&background_clr,fb,parlcd_mem_base);
  while (1) {
    knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    
    // in progress
    // *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
    // for (int i = 0; i < 32; i++) {
    //   if (val_line >= (1 << 32)) val_line >>= 1;
    //   else if (val_line <= 1) val_line <<= 1;
    // }

    change_RGB_lights(mem_base, clr);
    
    if ((knobs&0x07000000)==0x07000000) {
      //Turn off if all pressed
      printf("The program has ended\n");
      break;
    }

    if ((knobs&0x07000000)==0x02000000) {
      //change color if G pressed
      set_brush_color(mem_base,&clr,&delta_knobs);
      knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    }

    if ((knobs&0x07000000)==0x05000000) {
      // Clear canvas if R&B pressed
      clear_buffer(fb,background_clr);
      update_canvas(fb,parlcd_mem_base);
      printf("The canvas is cleared\n");
    }
    int pos_knobs = knobs-delta_knobs;
    xx = ((pos_knobs&0xff)*480)/256;
    yy = (((pos_knobs>>16)&0xff)*320)/256;

    for (int j=0; j<5; j++)
      for (int i=0; i<5; i++) 
        draw_pixel(fb,i+xx,j+yy,clr);
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