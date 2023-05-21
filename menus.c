#include "menus.h"

font_descriptor_t *fdes = &font_rom8x16;

//Function updates state on the led line to create animation in main menu
void menu_animation(unsigned char *mem_base,int* x,int* dir){
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = *x;
    if(*dir == 1)
        (*x)<<=1;
    else
        (*x)>>=1;
    if(*x == 255 || !( (*x) & (16777215)) )(*dir) ^= 1;
    return;
}

//Function updates state on the led line to create animation while drawing on canvas
void drawing_animation(unsigned char *mem_base){
   unsigned int v = 0;
   int x;
   clock_t currentTime;
   currentTime = clock();
   srand((unsigned int)currentTime);
   int num = rand()%8;
   for(int i = 0;i < num;i++){
        x = rand()%32;
        v |= (1<<x);
   }
   *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = v;
   return;
}

//Function creates main menu and puts it on the screen
void draw_main_menu(unsigned char *mem_base, void* parlcd_mem_base, unsigned short *fb) {
  my_sleep(500); // Sleep is needed to wait until the user releases the button
  //Initializing
  int val_line = 255;
  int dir = 1;
  unsigned short cur_state = START;
  char* string = (char*)malloc(30);
  unsigned short g;
  display_LED_lights(mem_base,0,0,0);//Turn off LED color indicators
  while (1) {
    menu_animation(mem_base,&val_line,&dir); //Upd LED line
    int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    g = (knobs>>8) & 255; //get value of the green knob
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
    
    //Draw menu elements
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

    if ((knobs&0x07000000)==0x02000000){ //Start drawing programm when G pressed
      my_sleep(1);
      switch(cur_state) {
        case START:
          start_drawing(mem_base,parlcd_mem_base,fb);
          break;
        case TUTORIAL:
          show_tutorial(mem_base,parlcd_mem_base,fb);
          break;
        case EXIT:
          //Turn off everything
          *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0;
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

//Get RGB background from the user and put it on the screen
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

  /*
  Deltas needed to display original values in order to display relevant values
  so there will be displayed 0 0 0.
  */
  dr = (256 - ((cur_knobs>>16) & 255)) % 256;
  dg = (256 - ((cur_knobs>>8) & 255)) % 256;
  db = (256 - (cur_knobs & 255)) % 256;

  while (1) {
    int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    r = (((knobs>>16) & 255)+dr) % 256;
    g = (((knobs>>8) & 255)+dg) % 256;
    b = ((knobs & 255)+db) % 256;
    display_LED_lights(mem_base,r,g,b);
    //Draw GUI
    draw_rectangle(fb,80,60,320,200,0x0000);
    string[0] = 'R';string[2] = r/100+48;string[3] = (r/10)%10+48;string[4] = r%10+48;
    draw_string(fb,110,60,string,0xffff,fdes,4);
    string[0] = 'G';string[2] = g/100+48;string[3] = (g/10)%10+48;string[4] = g%10+48;
    draw_string(fb,110,130,string,0xffff,fdes,4);
    string[0] = 'B';string[2] = b/100+48;string[3] = (b/10)%10+48;string[4] = b%10+48;
    draw_string(fb,110,200,string,0xffff,fdes,4);
    update_canvas(fb,parlcd_mem_base);
    my_sleep(1);

    //Leave function if G button is pressed again
    if ((knobs&0x07000000)==0x02000000){
      my_sleep(500);
      
      convert_RGB_to_RGB565(background_clr, r, g, b);
      printf("color is changed to r:%d g:%d b:%d\n",r,g,b);
      return;
    }
  }
}

//Get new brush size from the user and set it
void set_brush_size(unsigned char *mem_base,void *parlcd_mem_base,unsigned short *fb,unsigned short* clr,short* brush_size,int* delta_knobs){
  draw_rectangle(fb,80,60,320,200,0xFFFF);
  my_sleep(500);

  unsigned short clr_to_display = *clr;

  char* string = (char*)malloc(30);
  strncpy(string, "SIZE:\0", 6);

  unsigned short g;
  int cur_knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  cur_knobs &= (16777215);
  //Delta is needed to save old value so user can simply exit setting without changing
  short orig = ((cur_knobs>>8) & 255) - *brush_size;

  while (1) {
    int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    g = (knobs>>8) & 255;
    *brush_size = (255 + g - orig)%51;
    if(*brush_size < 5)*brush_size = 5;

    //Draw GUI
    draw_rectangle(fb,80,60,320,200,0xFFFF);
    string[5] = ((*brush_size)/10)%10+48;string[6] = (*brush_size)%10+48;
    //If color is close to white, show it as black
    if (clr_to_display >= 0xFFFA && clr_to_display <= 0xFFFF) clr_to_display = 0x0000;
    draw_rectangle(fb, 207, 120, (int)*brush_size, (int)*brush_size, clr_to_display);
    draw_string(fb,165,220,string,0x0000,fdes,2);
    update_canvas(fb,parlcd_mem_base);
    my_sleep(1);

    //Leave function if G button is pressed again
    if ((knobs&0x07000000)==0x02000000){
      my_sleep(500);
      knobs &= (16777215);
      printf("brush size changed to :%d\n",(*brush_size));
      *delta_knobs += knobs - cur_knobs;
      return;
    }
  }
}

//Get new brush color from the user and set it
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

  /*
  Deltas needed to display original values in order to display relevant values
  so user can exit the function without changes brush color
  */
  dr = (256 + old_r - ((cur_knobs>>16) & 255)) % 256;
  dg = (256 + old_g - ((cur_knobs>>8) & 255)) % 256;
  db = (256 + old_b - (cur_knobs & 255)) % 256;

  while (1) {
    int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    r = (((knobs>>16) & 255)+dr) % 256;
    g = (((knobs>>8) & 255)+dg) % 256;
    b = ((knobs & 255)+db) % 256;
    display_LED_lights(mem_base,r,g,b);
    
    //Draw GUI
    draw_rectangle(fb,80,60,320,200,0xFFFF);
    string[0] = 'R';string[2] = r/100+48;string[3] = (r/10)%10+48;string[4] = r%10+48;
    draw_string(fb,110,60,string,0x0000,fdes,4);
    string[0] = 'G';string[2] = g/100+48;string[3] = (g/10)%10+48;string[4] = g%10+48;
    draw_string(fb,110,130,string,0x0000,fdes,4);
    string[0] = 'B';string[2] = b/100+48;string[3] = (b/10)%10+48;string[4] = b%10+48;
    draw_string(fb,110,200,string,0x0000,fdes,4);
    update_canvas(fb,parlcd_mem_base);
    my_sleep(1);

    if ((knobs&0x07000000)==0x02000000){ // Exit funciton
      my_sleep(500);
      
      knobs &= (16777215);
      convert_RGB_to_RGB565(clr, r, g, b);
      *delta_knobs += knobs - cur_knobs;
      old_r = r,old_g = g,old_b = b;
      set_brush_size(mem_base,parlcd_mem_base,fb,clr,brush_size,delta_knobs);
      return;
    }
  }
}

//Function calls brush-configuration functions, then returns to frame buffer with drawing
void configurate_brush(unsigned char *mem_base,void *parlcd_mem_base,unsigned short *fb,unsigned short *old_fb,unsigned short* clr,short* brush_size,int* delta_knobs,int* knobs){
    memcpy(old_fb,fb,BUFFERS_LEN); //Save current buffer
    set_brush_color(mem_base,parlcd_mem_base,fb,clr,brush_size,delta_knobs);
    memcpy(fb,old_fb,BUFFERS_LEN); //Write original frame buffer
    *knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);//Update current knobs value
    return;
}

//Function is used to clear users drawing
void clear_canvas(void *parlcd_mem_base,unsigned short *fb,int background_color){
    fill_buffer(fb,background_color);
    update_canvas(fb,parlcd_mem_base);
}

//Function shows canvas on screen for user to draw and handles controls
void start_drawing(unsigned char *mem_base, void* parlcd_mem_base, unsigned short *fb)
{
  /*
  Delta is used to save brush position because when user calls brush
  configuration values, knobs values becomes irrelevant and changes brush position
  */
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
    drawing_animation(mem_base);
    knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    
    if ((knobs&0x07000000)==0x01000000) //Blue button - go back
        return;
    if ((knobs&0x07000000)==0x02000000) // Green button - configurate brush
      configurate_brush(mem_base,parlcd_mem_base,fb,old_fb,&clr,&brush_size,&delta_knobs,&knobs);
    if ((knobs&0x07000000)==0x04000000) // Red button - clear drawing
        clear_canvas(parlcd_mem_base,fb,background_color);

    int pos_knobs = knobs-delta_knobs;
    xx = ((pos_knobs&0xff)*480)/256;
    yy = (((pos_knobs>>16)&0xff)*320)/256;

    //Clear the gap
    connect_dots(fb, xx, yy,pr_xx,pr_yy, brush_size, brush_size, clr);
    pr_xx = xx;
    pr_yy = yy;
    update_canvas(fb,parlcd_mem_base);
 
    my_sleep(1);
  }
}

//Function shows tutorial on the screen
void show_tutorial(unsigned char *mem_base, void* parlcd_mem_base, unsigned short *fb){
    //Draw strings
    fill_buffer(fb,0);
    update_canvas(fb,parlcd_mem_base);
    char* string = (char*)malloc(30);
    memset(string,0,30);
    strncpy(string, "Red button:\0", 12);
    draw_string(fb,10,10,string,0xf800,fdes,2);
    memset(string,0,30);
    strncpy(string, "clear canvas\0", 13);
    draw_string(fb,10,40,string,0xf800,fdes,2);

    memset(string,0,30);
    strncpy(string, "Green button:\0", 14);
    draw_string(fb,10,105,string,0x7e0,fdes,2);
    memset(string,0,30);
    strncpy(string, "brush configuration/OK\0", 28);
    draw_string(fb,10,135,string,0x7e0,fdes,2);

    memset(string,0,30);
    strncpy(string, "Blue button:\0", 13);
    draw_string(fb,10,195,string,0x7ff,fdes,2);
    memset(string,0,30);
    strncpy(string, "main menu\0", 10);
    draw_string(fb,10,225,string,0x7ff,fdes,2);

    update_canvas(fb,parlcd_mem_base);
    while (1) {
        int knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
        if ((knobs&0x07000000)==0x01000000) return; // Blue button - go back
        my_sleep(1);
    }
}
