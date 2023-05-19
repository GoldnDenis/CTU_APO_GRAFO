  /*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by 

  main.c - main file

  include your name there and license for distribution.

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L
#define BUFFERS_LEN 320*480*2
 
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
#include <string.h>
 
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "draw_utils.h"
#include "menus.h"
#include "timers.h"

int main(int argc, char *argv[]) 
{

  int delta_knobs = 0;

  unsigned char *parlcd_mem_base, *mem_base;
  unsigned short clr = 0xffff; // "WHITE"
  short brush_size = 5;
  unsigned short background_clr = 0x0000;
  unsigned short *fb  = (unsigned short *)malloc(BUFFERS_LEN);
  unsigned short *old_fb  = (unsigned short *)malloc(BUFFERS_LEN);
 
  printf("Hello world\n");
 
  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
  if (parlcd_mem_base == NULL)
    exit(1);
 
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
  if (mem_base == NULL)
    exit(1);
 
  parlcd_hx8357_init(parlcd_mem_base);
 
  draw_main_menu(mem_base, parlcd_mem_base, fb, &background_clr);

  int xx=0, yy=0,pr_xx = 0,pr_yy = 0;
  int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  delta_knobs += knobs;
  display_LED_lights(mem_base,255,255,255);
  while (1) {
    knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    
    if ((knobs&0x07000000)==0x01000000) {
      // B pressed
      printf("Going back to the Menu\n");

      clr = 0xffff; // "WHITE"
      brush_size = 5;
      background_clr = 0;

      draw_main_menu(mem_base, parlcd_mem_base, fb, &background_clr);

      knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
      xx = yy = 0;
      delta_knobs = knobs;
      continue;
    }

    if ((knobs&0x07000000)==0x02000000) {
      //change color if G pressed
      memcpy(old_fb,fb,BUFFERS_LEN);
      set_brush_color(mem_base,parlcd_mem_base,fb,&clr,&brush_size,&delta_knobs);
      memcpy(fb,old_fb,BUFFERS_LEN);
      knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    }

    if ((knobs&0x07000000)==0x04000000) {
      // Clear canvas if R pressed
      fill_buffer(fb,background_clr);
      update_canvas(fb,parlcd_mem_base);
      printf("The canvas is cleared\n");
    }
    int pos_knobs = knobs-delta_knobs;
    xx = ((pos_knobs&0xff)*480)/256;
    yy = (((pos_knobs>>16)&0xff)*320)/256;

    connect_dots(fb, xx, yy,pr_xx,pr_yy, brush_size, brush_size, clr);
    pr_xx = xx;
    pr_yy = yy;
    update_canvas(fb,parlcd_mem_base);
 
    my_sleep(1);
  }
 
  return 0;
}
