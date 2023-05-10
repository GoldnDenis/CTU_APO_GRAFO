/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON

  change_me.c      - main file

  include your name there and license for distribution.

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"

int main(int argc, char *argv[]) {
  unsigned char *mem_base;
  uint32_t val_line=5,knobs = 0;
  int i;
  printf("Hello world\n");

  sleep(1);

  /*
   * Setup memory mapping which provides access to the peripheral
   * registers region of RGB LEDs, knobs and line of yellow LEDs.
   */
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);

  /* If mapping fails exit with error code */
  if (mem_base == NULL)
    exit(1);

  struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 20 * 1000 * 1000};
  int left = 1; 
  while( /*!(knobs & 0x07000000)*/ 1 ) {
     knobs = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
     int shift = (((knobs>>16) & 0xff) / 4) % 32;
    //  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line<<shift;
     *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = knobs;
     printf("LED val 0x%x\n", val_line<<shift);
     *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = knobs;
     clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
  }
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0;
  printf("Goodbye world\n");

  return 0;
}
