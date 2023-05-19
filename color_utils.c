#include "color_utils.h"

// change color to LED format (RGB)
void unite_RGB(unsigned int *color, uint8_t r, uint8_t g, uint8_t b){
  uint32_t val = 0;
  val |= r;
  val <<= 8;
  val |= g;
  val <<= 8;
  val |= b;

  *color = val;
  return;
}

void display_LED_lights(unsigned char *mem_base, unsigned short r, unsigned short g, unsigned short b) {
  unsigned int tmp_clr = 0;
  unite_RGB(&tmp_clr, r, g, b);

  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = tmp_clr;
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = tmp_clr;
}

// change color to LCD format
void convert_RGB_to_LCD(unsigned short *color, uint8_t r, uint8_t g, uint8_t b){
  uint32_t val = 0;
  val |= (r>>3);
  val <<= 6;
  val |= (g>>2);
  val <<= 5;
  val |= (b>>3);

  *color = val;
  return;
} 
