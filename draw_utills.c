#include "draw_utills.h"

void draw_pixel(unsigned short *fb,int x, int y, unsigned short color){
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

// Change the color of RGB-lights
void change_RGB_lights(unsigned char *mem_base, unsigned int clr) {
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

void change_tmp_color(unsigned char *mem_base, unsigned short r, unsigned short g, unsigned short b) {
  unsigned int tmp_clr = 0;
  change_color_LED(&tmp_clr, r, g, b);

  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = tmp_clr;
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = tmp_clr;
}

void set_brush_color(unsigned char *mem_base,unsigned short* clr,int* delta_knobs){
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
    change_tmp_color(mem_base,r,g,b);

    if ((knobs&0x07000000)==0x02000000){
      clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
      
      knobs &= (16777215);
      change_color_LCD(clr, r, g, b);
      *delta_knobs += knobs - cur_knobs;
      printf("color is changed to r:%d g:%d b:%d quit set_color fnc\n",r,g,b);
      return;
    }
  }
}

void set_background_color(unsigned short* background_color,unsigned short *fb,void *parlcd_mem_base){
  *background_color = 0x07E0;
  clear_buffer(fb,*background_color);
  update_canvas(fb,parlcd_mem_base);
  return;
}
