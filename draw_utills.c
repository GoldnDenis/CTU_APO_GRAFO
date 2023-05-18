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

void draw_pixel_big(unsigned short *fb,int x, int y, unsigned short color,int scale) {
  int i,j;
  for (i=0; i<scale; i++) {
    for (j=0; j<scale; j++) {
      draw_pixel(fb,x+i, y+j, color);
    }
  }
}

int char_width(font_descriptor_t *fdes,int ch) {
  int width;
  if (!fdes->width) {
    width = fdes->maxwidth;
  } else {
    width = fdes->width[ch-fdes->firstchar];
  }
  return width;
}

void draw_char(unsigned short *fb,int x, int y, char ch, unsigned short color,font_descriptor_t *fdes,int scale) {
  int w = char_width(fdes,ch);
  const font_bits_t *ptr;
  if ((ch >= fdes->firstchar) && (ch-fdes->firstchar < fdes->size)) {
    if (fdes->offset) {
      ptr = &fdes->bits[fdes->offset[ch-fdes->firstchar]];
    } else {
      int bw = (fdes->maxwidth+15)/16;
      ptr = &fdes->bits[(ch-fdes->firstchar)*bw*fdes->height];
    }
    int i, j;
    for (i=0; i<fdes->height; i++) {
      font_bits_t val = *ptr;
      for (j=0; j<w; j++) {
        if ((val&0x8000)!=0) {
          draw_pixel_big(fb,x+scale*j, y+scale*i, color,scale);
        }
        val<<=1;
      }
      ptr++;
    }
  }
}

void draw_rectangle(unsigned short *fb,int x,int y,int w,int h,unsigned short color){
  for (int j = 0;j < h;j++)
      for (int i = 0;i < w;i++) 
        draw_pixel(fb,x+i,y+j,color);
  return;
}

void draw_string(unsigned short *fb,int x, int y, char* string, unsigned short color,font_descriptor_t *fdes,int scale){
  int dx = 0;
  int sz = strlen(string);
  for(int i = 0;i < sz;i++){
    draw_char(fb,x+dx,y,string[i],color,fdes,scale);
    dx += 8*scale+5;
  }
}

void set_background_color(unsigned char *mem_base,void *parlcd_mem_base,unsigned short *fb,unsigned short* background_clr){
  struct timespec stop_delay;
  stop_delay.tv_sec = 0;
  stop_delay.tv_nsec = 500 * 1000 * 1000; // 500 ms
  draw_rectangle(fb,80,60,320,200,0xFFFF);
  clock_nanosleep(CLOCK_MONOTONIC, 0, &stop_delay, NULL);
  
  font_descriptor_t *fdes = &font_rom8x16;

  char* string = (char*)malloc(30);
  string[1] = ':';
  string[5] = 0;

  struct timespec loop_delay;
  loop_delay.tv_sec = 0;
  loop_delay.tv_nsec = 1 * 1000 * 1000; // 500 ms

  unsigned short r,g,b;
  int cur_knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  cur_knobs &= (16777215);

  while (1) {
    int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    r = (knobs>>16) & 255;
    g = (knobs>>8) & 255;
    b = knobs & 255;
    change_tmp_color(mem_base,r,g,b);

    printf("R:%d G:%d B:%d \n",r,g,b);
    draw_rectangle(fb,80,60,320,200,0x0000);
    string[0] = 'R';string[2] = r/100+48;string[3] = (r/10)%10+48;string[4] = r%10+48;
    draw_string(fb,110,60,string,0xffff,fdes,4);
    string[0] = 'G';string[2] = g/100+48;string[3] = (g/10)%10+48;string[4] = g%10+48;
    draw_string(fb,110,130,string,0xffff,fdes,4);
    string[0] = 'B';string[2] = b/100+48;string[3] = (b/10)%10+48;string[4] = b%10+48;
    draw_string(fb,110,200,string,0xffff,fdes,4);
    update_canvas(fb,parlcd_mem_base);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);

    if ((knobs&0x07000000)==0x02000000){
      clock_nanosleep(CLOCK_MONOTONIC, 0, &stop_delay, NULL);
      
      change_color_LCD(background_clr, r, g, b);
      printf("color is changed to r:%d g:%d b:%d\n",r,g,b);
      return;
    }
  }
}

void draw_main_menu(unsigned char *mem_base, void* parlcd_mem_base, unsigned short *fb, unsigned short *background_color) {
  struct timespec stop_delay;
  stop_delay.tv_sec = 0;
  stop_delay.tv_nsec = 500 * 1000 * 1000; // 500 ms
  clock_nanosleep(CLOCK_MONOTONIC, 0, &stop_delay, NULL);
  
  font_descriptor_t *fdes = &font_rom8x16;

  unsigned short cur_state = START;
  char* string = (char*)malloc(30);

  struct timespec loop_delay;
  loop_delay.tv_sec = 0;
  loop_delay.tv_nsec = 1 * 1000 * 1000; // 500 ms

  unsigned short g;
  while (1) {
    int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    g = (knobs>>8) & 255;

    clear_buffer(fb, 0xffff);
    
    // NEEDS TO BE CHANGED, BASIC IMPL for TESTING
    cur_state = g % 3;

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

    string[0] = 'A';string[1] = 'P';string[2] = 'O';string[3] = ':';
    string[4] = 'G';string[5] = 'R';string[6] = 'A';string[7] = 'F';string[8] = 'O';
    draw_string(fb,40,45,string,0x0000,fdes,5);
    draw_string(fb,43,48,string,0xeeee,fdes,5);

    string[5] = '\0';
    string[0] = 'S';string[1] = 'T';string[2] = 'A';string[3] = 'R';string[4] = 'T';
    draw_string(fb, 140, 135, string, 0x0000, fdes, 3);
    string[8] = '\0';

    string[0] = 'T';string[1] = 'U';string[2] = 'T';string[3] = 'O';string[4] = 'R';string[5] = 'I';string[6] = 'A';string[7] = 'L';
    draw_string(fb, 140, 185, string, 0x0000, fdes, 3);
    string[4] = '\0';

    string[0] = 'E';string[1] = 'X';string[2] = 'I';string[3] = 'T';
    draw_string(fb, 140, 235, string, 0x0000, fdes, 3);

    update_canvas(fb,parlcd_mem_base);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);

    if ((knobs&0x07000000)==0x02000000){
      clock_nanosleep(CLOCK_MONOTONIC, 0, &stop_delay, NULL);
      switch(cur_state) {
        case START:
        // to do
          set_background_color(mem_base, parlcd_mem_base, fb, background_color);
          clear_buffer(fb, *background_color);
          return;
          break;
        case TUTORIAL:
          // TO-DO
          break;
        case EXIT:
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

void set_brush_size(unsigned char *mem_base,void *parlcd_mem_base,unsigned short *fb,unsigned short* clr,short* brush_size,int* delta_knobs){
  struct timespec stop_delay;
  stop_delay.tv_sec = 0;
  stop_delay.tv_nsec = 500 * 1000 * 1000; // 500 ms
  draw_rectangle(fb,80,60,320,200,0xFFFF);
  clock_nanosleep(CLOCK_MONOTONIC, 0, &stop_delay, NULL);
  
  font_descriptor_t *fdes = &font_rom8x16;
  unsigned short clr_to_display = *clr;

  char* string = (char*)malloc(30);
  string[0] = 'S';string[1] = 'I';string[2] = 'Z';string[3] = 'E';string[4] = ':';

  struct timespec loop_delay;
  loop_delay.tv_sec = 0;
  loop_delay.tv_nsec = 1 * 1000 * 1000; // 500 ms

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
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);

    if ((knobs&0x07000000)==0x02000000){
      clock_nanosleep(CLOCK_MONOTONIC, 0, &stop_delay, NULL);
      knobs &= (16777215);
      printf("brush size changed to :%d\n",(*brush_size));
      *delta_knobs += knobs - cur_knobs;
      return;
    }
  }
}

void set_brush_color(unsigned char *mem_base,void *parlcd_mem_base,unsigned short *fb,unsigned short* clr,short* brush_size,int* delta_knobs){
  //TODO - remember old rgb setup (like in set_brush_size).
  struct timespec stop_delay;
  stop_delay.tv_sec = 0;
  stop_delay.tv_nsec = 500 * 1000 * 1000; // 500 ms
  draw_rectangle(fb,80,60,320,200,0xFFFF);
  clock_nanosleep(CLOCK_MONOTONIC, 0, &stop_delay, NULL);
  
  font_descriptor_t *fdes = &font_rom8x16;

  char* string = (char*)malloc(30);
  string[1] = ':';
  string[5] = 0;

  struct timespec loop_delay;
  loop_delay.tv_sec = 0;
  loop_delay.tv_nsec = 1 * 1000 * 1000; // 500 ms

  unsigned short r,g,b;
  int cur_knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  cur_knobs &= (16777215);

  while (1) {
    int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    r = (knobs>>16) & 255;
    g = (knobs>>8) & 255;
    b = knobs & 255;
    change_tmp_color(mem_base,r,g,b);

    printf("R:%d G:%d B:%d \n",r,g,b);
    draw_rectangle(fb,80,60,320,200,0xFFFF);
    string[0] = 'R';string[2] = r/100+48;string[3] = (r/10)%10+48;string[4] = r%10+48;
    draw_string(fb,110,60,string,0x0000,fdes,4);
    string[0] = 'G';string[2] = g/100+48;string[3] = (g/10)%10+48;string[4] = g%10+48;
    draw_string(fb,110,130,string,0x0000,fdes,4);
    string[0] = 'B';string[2] = b/100+48;string[3] = (b/10)%10+48;string[4] = b%10+48;
    draw_string(fb,110,200,string,0x0000,fdes,4);
    update_canvas(fb,parlcd_mem_base);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);

    if ((knobs&0x07000000)==0x02000000){
      clock_nanosleep(CLOCK_MONOTONIC, 0, &stop_delay, NULL);
      
      knobs &= (16777215);
      change_color_LCD(clr, r, g, b);
      *delta_knobs += knobs - cur_knobs;
      printf("color is changed to r:%d g:%d b:%d quit set_color fnc\n",r,g,b);
      set_brush_size(mem_base,parlcd_mem_base,fb,clr,brush_size,delta_knobs);
      return;
    }
  }
}
