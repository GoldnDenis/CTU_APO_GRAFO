#include "menus.h"

font_descriptor_t *fdes = &font_rom8x16;

void draw_main_menu(unsigned char *mem_base, void* parlcd_mem_base, unsigned short *fb) {
  my_sleep(500);
  unsigned short cur_state = START;
  char* string = (char*)malloc(30);
  unsigned short g;
  display_LED_lights(mem_base,0,0,0);
  while (1) {
    int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    g = (knobs>>8) & 255;
    fill_buffer(fb, 0xffff);
    cur_state = (g/4) % 3; //Lower sensivity in main menu
    switch(cur_state) {
      case START:
        draw_char(fb, 110, 135, '>', 0x0000, fdes, 3);
        break;
      case TUTORIAL:
        draw_char(fb, 110, 185, '>', 0x0000, fdes, 3);
        break;
      case EXIT:
        draw_char(fb, 110, 235, '>', 0x0000, fdes, 3);
        break;
    }
    memset(string,0,30);
    strncpy(string, "APO:GRAFO\0", 10);
    draw_string(fb,40,45,string,0x0000,fdes,5);
    draw_string(fb,43,48,string,0xeeee,fdes,5);

    memset(string,0,30);
    strncpy(string, "START\0", 6);
    draw_string(fb, 140, 135, string, 0x0000, fdes, 3);

    memset(string,0,30);
    strncpy(string, "TUTORIAL\0", 9);
    draw_string(fb, 140, 185, string, 0x0000, fdes, 3);

    memset(string,0,30);
    strncpy(string, "EXIT\0", 5);
    draw_string(fb, 140, 235, string, 0x0000, fdes, 3);

    update_canvas(fb,parlcd_mem_base);
    my_sleep(1);

    if ((knobs&0x07000000)==0x02000000){
      my_sleep(1);
      switch(cur_state) {
        case START:
          start_drawing(mem_base,parlcd_mem_base,fb);
          break;
        case TUTORIAL:
          // TO-DO
          break;
        case EXIT:
          display_LED_lights(mem_base,0,0,0);
          parlcd_write_cmd(parlcd_mem_base, 0x2c);
          for (int ptr = 0; ptr < 480*320 ; ptr++) {
            parlcd_write_data(parlcd_mem_base, 0);
          }
          exit(0);
          break;
      }
    }
  }
}

void set_background_color(unsigned char *mem_base,void *parlcd_mem_base,unsigned short *fb,unsigned short* background_clr){    
  draw_rectangle(fb,80,60,320,200,0xFFFF);  
  my_sleep(500);

  char* string = (char*)malloc(30);
  string[1] = ':';
  string[5] = 0;

  unsigned short r,g,b;
  unsigned short dr,dg,db;
  int cur_knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  cur_knobs &= (16777215);

  dr = (256 - ((cur_knobs>>16) & 255)) % 256;
  dg = (256 - ((cur_knobs>>8) & 255)) % 256;
  db = (256 - (cur_knobs & 255)) % 256;

  while (1) {
    int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    r = (((knobs>>16) & 255)+dr) % 256;
    g = (((knobs>>8) & 255)+dg) % 256;
    b = ((knobs & 255)+db) % 256;
    display_LED_lights(mem_base,r,g,b);
    draw_rectangle(fb,80,60,320,200,0x0000);
    string[0] = 'R';string[2] = r/100+48;string[3] = (r/10)%10+48;string[4] = r%10+48;
    draw_string(fb,110,60,string,0xffff,fdes,4);
    string[0] = 'G';string[2] = g/100+48;string[3] = (g/10)%10+48;string[4] = g%10+48;
    draw_string(fb,110,130,string,0xffff,fdes,4);
    string[0] = 'B';string[2] = b/100+48;string[3] = (b/10)%10+48;string[4] = b%10+48;
    draw_string(fb,110,200,string,0xffff,fdes,4);
    update_canvas(fb,parlcd_mem_base);
    my_sleep(1);

    if ((knobs&0x07000000)==0x02000000){
      my_sleep(500);
      
      convert_RGB_to_LCD(background_clr, r, g, b);
      printf("color is changed to r:%d g:%d b:%d\n",r,g,b);
      return;
    }
  }
}

void set_brush_size(unsigned char *mem_base,void *parlcd_mem_base,unsigned short *fb,unsigned short* clr,short* brush_size,int* delta_knobs){
  draw_rectangle(fb,80,60,320,200,0xFFFF);
  my_sleep(500);

  unsigned short clr_to_display = *clr;

  char* string = (char*)malloc(30);
  strncpy(string, "SIZE:\0", 6);

  unsigned short g;
  int cur_knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  cur_knobs &= (16777215);
  short orig = ((cur_knobs>>8) & 255) - *brush_size;

  while (1) {
    int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    g = (knobs>>8) & 255;    
    *brush_size = (255 + g - orig)%51;
    if(*brush_size < 5)*brush_size = 5;

    draw_rectangle(fb,80,60,320,200,0xFFFF);

    string[5] = ((*brush_size)/10)%10+48;string[6] = (*brush_size)%10+48;
    if (clr_to_display >= 0xFFFA && clr_to_display <= 0xFFFF) clr_to_display = 0x0000;
    draw_rectangle(fb, 207, 120, (int)*brush_size, (int)*brush_size, clr_to_display);
    draw_string(fb,165,220,string,0x0000,fdes,2);
    update_canvas(fb,parlcd_mem_base);
    my_sleep(1);

    if ((knobs&0x07000000)==0x02000000){
      my_sleep(500);
      knobs &= (16777215);
      printf("brush size changed to :%d\n",(*brush_size));
      *delta_knobs += knobs - cur_knobs;
      return;
    }
  }
}

void set_brush_color(unsigned char *mem_base,void *parlcd_mem_base,unsigned short *fb,unsigned short* clr,short* brush_size,int* delta_knobs){
  draw_rectangle(fb,80,60,320,200,0xFFFF);
  my_sleep(500);

  char* string = (char*)malloc(30);
  string[1] = ':';
  string[5] = 0;

  static unsigned short old_r=255,old_g=255,old_b=255;
  unsigned short dr,dg,db;
  unsigned short r,g,b;
  int cur_knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  cur_knobs &= (16777215);

  dr = (256 + old_r - ((cur_knobs>>16) & 255)) % 256;
  dg = (256 + old_g - ((cur_knobs>>8) & 255)) % 256;
  db = (256 + old_b - (cur_knobs & 255)) % 256;

  while (1) {
    int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    r = (((knobs>>16) & 255)+dr) % 256;
    g = (((knobs>>8) & 255)+dg) % 256;
    b = ((knobs & 255)+db) % 256;
    display_LED_lights(mem_base,r,g,b);

    draw_rectangle(fb,80,60,320,200,0xFFFF);
    string[0] = 'R';string[2] = r/100+48;string[3] = (r/10)%10+48;string[4] = r%10+48;
    draw_string(fb,110,60,string,0x0000,fdes,4);
    string[0] = 'G';string[2] = g/100+48;string[3] = (g/10)%10+48;string[4] = g%10+48;
    draw_string(fb,110,130,string,0x0000,fdes,4);
    string[0] = 'B';string[2] = b/100+48;string[3] = (b/10)%10+48;string[4] = b%10+48;
    draw_string(fb,110,200,string,0x0000,fdes,4);
    update_canvas(fb,parlcd_mem_base);
    my_sleep(1);

    if ((knobs&0x07000000)==0x02000000){
      my_sleep(500);
      
      knobs &= (16777215);
      convert_RGB_to_LCD(clr, r, g, b);
      *delta_knobs += knobs - cur_knobs;
      printf("color is changed to r:%d g:%d b:%d quit set_color fnc\n",r,g,b);
      old_r = r,old_g = g,old_b = b;
      set_brush_size(mem_base,parlcd_mem_base,fb,clr,brush_size,delta_knobs);
      return;
    }
  }
}
