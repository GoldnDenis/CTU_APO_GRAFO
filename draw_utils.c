#include "draw_utils.h"

void draw_pixel(unsigned short *fb,int x, int y, unsigned short color){
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = color;  
  }
  return;
}

void fill_buffer(unsigned short *fb,unsigned int background_color){
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

void connect_dots(unsigned short *fb,int x,int y,int px,int py,int w,int h,unsigned short color){
  draw_rectangle(fb, px, py, w, h, color);
  draw_rectangle(fb, x, y, w, h, color);
  if(abs(px-x) > 200){
    if(x <= 100)px = 0;
    else px = 480;
  }
  if(abs(py-y) > 200){
    if(y <= 100)py = 0;
    else py = 320;
  }
  int cx=px,cy=py;
  while( (cx != x) || (cy != y) ){
    draw_rectangle(fb, cx, cy, w, h, color);
    if(cx < x)cx++;
    else if(cx > x) cx--;
    if(cy < y)cy++;
    else if(cy > y) cy--;
  }
  return;
}

void draw_string(unsigned short *fb,int x, int y, char* string, unsigned short color,font_descriptor_t *fdes,int scale){
  int dx = 0;
  int sz = strlen(string);
  for(int i = 0;i < sz;i++){
    draw_char(fb,x+dx,y,string[i],color,fdes,scale);
    dx += 8*scale+5;
  }
  return;
}
