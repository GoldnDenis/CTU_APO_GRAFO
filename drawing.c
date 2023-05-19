#include "drawing.h"

void start_drawing(unsigned char *mem_base, void* parlcd_mem_base, unsigned short *fb)
{
  int delta_knobs = 0;
  unsigned short clr = 0xffff; // "WHITE"
  short brush_size = 5;
  unsigned short background_color = 0x0000;
  unsigned short *old_fb  = (unsigned short *)malloc(BUFFERS_LEN);
  set_background_color(mem_base, parlcd_mem_base, fb, &background_color);
  fill_buffer(fb, background_color);

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
      background_color = 0;

      draw_main_menu(mem_base, parlcd_mem_base, fb);

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
      fill_buffer(fb,background_color);
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
}