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
 
void draw_pixel(int x, int y, unsigned short color) {
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = color;
  }
}
 
 
int main(int argc, char *argv[]) {
  unsigned char *parlcd_mem_base, *mem_base;
  int i,j;
  int ptr;
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
 
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  ptr=0;
  for (i = 0; i < 320 ; i++) {
    for (j = 0; j < 480 ; j++) {
      c = 0;
      fb[ptr]=c;
      parlcd_write_data(parlcd_mem_base, fb[ptr++]);
    }
  }
 
  struct timespec loop_delay;
  loop_delay.tv_sec = 0;
  loop_delay.tv_nsec = 150 * 1000 * 1000;
  int xx=0, yy=0;
  while (1) {
 
    int r = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    if ((r&0x7000000)!=0) {
      break;
    }
    xx = ((r&0xff)*480)/256;
    yy = (((r>>8)&0xff)*320)/256;
 
    for (ptr = 0; ptr < 320*480 ; ptr++) {
        fb[ptr]=0u;
    }
    for (j=0; j<60; j++) {
      for (i=0; i<60; i++) {
        draw_pixel(i+xx,j+yy,0x7ff);
      }
    }
 
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }
 
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
  }
 
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (ptr = 0; ptr < 480*320 ; ptr++) {
    parlcd_write_data(parlcd_mem_base, 0);
  }
 
  printf("Goodbye world\n");
 
  return 0;
}