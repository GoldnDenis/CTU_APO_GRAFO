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
int delta_knobs;


void draw_pixel(int x, int y, unsigned short color) {
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = color;  
  }
  return;
}

void clear_buffer(unsigned short *fb,unsigned int background_color){
  for (int ptr = 0; ptr < 320*480 ; ptr++) 
    fb[ptr]=background_color;
  return;
}

void update_canvas(unsigned short *fb,void *parlcd_mem_base){
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (int ptr = 0; ptr < 480*320 ; ptr++) {
      parlcd_write_data(parlcd_mem_base, fb[ptr]);
  }
  return;
}

//  ((r>>3)<<11)|((g>>2)<<5)|(b>>3) - parse1 color


// change color to LED format (RGB)
void change_color_LED(unsigned int *color, uint8_t r, uint8_t g, uint8_t b){
  uint32_t val = 0;
  val |= r;
  val <<= 8;
  val |= g;
  val <<= 8;
  val |= b;

  *color = val;
  return;
}

// Change RGB-lights color
void change_color_RGB(unsigned char *mem_base, unsigned int clr) {
  uint8_t r = (clr >> 11) & 0x1F;
  r <<= 3;
  uint8_t g = (clr >> 5) & 0x3F;
  g <<= 2;
  uint8_t b = clr & 0x1F;
  b <<= 3;
  unsigned int tmp_clr = 0;
  change_color_LED(&tmp_clr, r, g, b);

  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = tmp_clr;
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = tmp_clr;
}

void change_tmp(unsigned char *mem_base, unsigned short r, unsigned short g, unsigned short b) {
  unsigned int tmp_clr = 0;
  change_color_LED(&tmp_clr, r, g, b);

  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = tmp_clr;
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = tmp_clr;
}

// change color to LCD format
void change_color_LCD(unsigned short *color, uint8_t r, uint8_t g, uint8_t b){
  uint32_t val = 0;
  val |= (r>>3);
  val <<= 6;
  val |= (g>>2);
  val <<= 5;
  val |= (b>>3);

  *color = val;
  return;
} 

void set_color(unsigned char *mem_base,unsigned short* clr){
  struct timespec loop_delay;
  loop_delay.tv_sec = 0;
  loop_delay.tv_nsec = 500 * 1000 * 1000; // 500 ms
  clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);

  unsigned short r,g,b;
  int cur_knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  cur_knobs &= (16777215);

  while (1) {
    int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    r = (knobs>>16) & 255;
    g = (knobs>>8) & 255;
    b = knobs & 255;
    printf("R:%d G:%d B:%d \n",r,g,b);
    change_tmp(mem_base,r,g,b);

    if ((knobs&0x07000000)==0x02000000){
      clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
      
      knobs &= (16777215);
      change_color_LCD(clr, r, g, b);
      delta_knobs += knobs - cur_knobs;
      printf("color is changed to r:%d g:%d b:%d quit set_color fnc\n",r,g,b);
      return;
    }
  }
}

void set_backround(unsigned short* background_color,unsigned short *fb,void *parlcd_mem_base){
  *background_color = 0x07E0;
  clear_buffer(fb,*background_color);
  update_canvas(fb,parlcd_mem_base);
  return;
}
 
int main(int argc, char *argv[]) {
  uint32_t val_line = 1;

  unsigned char *parlcd_mem_base, *mem_base;
  int ptr;
  unsigned short clr = 0xffff; // "WHITE"
  unsigned short background_clr = 0;
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
 
  clear_buffer(fb,background_clr);
  update_canvas(fb,parlcd_mem_base);
  
  struct timespec loop_delay;
  loop_delay.tv_sec = 0;
  loop_delay.tv_nsec = 1 * 1000 * 1000;
  int xx=0, yy=0;
  int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  delta_knobs += knobs;
  set_backround(&background_clr,fb,parlcd_mem_base);
  while (1) {
    knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    
    // in progress
    // *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
    // for (int i = 0; i < 32; i++) {
    //   if (val_line >= (1 << 32)) val_line >>= 1;
    //   else if (val_line <= 1) val_line <<= 1;
    // }

    change_color_RGB(mem_base, clr);
    
    if ((knobs&0x07000000)==0x07000000) {
      //Turn off if all pressed
      printf("The program has ended\n");
      break;
    }

    if ((knobs&0x07000000)==0x02000000) {
      //change color if G pressed
      set_color(mem_base,&clr);
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