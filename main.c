  /*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by 

  main.c - main file

  Nazarov Rodion, Pak Denis

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L

#include "menus.h"

int main(int argc, char *argv[]) 
{
  //Initializing
  unsigned char *parlcd_mem_base, *mem_base;
  unsigned short *fb  = (unsigned short *)malloc(BUFFERS_LEN);
  
  printf("Hello world\n");
  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
  if (parlcd_mem_base == NULL)exit(1);
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
  if (mem_base == NULL)exit(1);
  parlcd_hx8357_init(parlcd_mem_base);
  draw_main_menu(mem_base, parlcd_mem_base, fb);
 
  return 0;
}
