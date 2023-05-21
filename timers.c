#include "timers.h"

//Sleep for t milliseconds
void my_sleep(int t){
  struct timespec stop_delay;
  stop_delay.tv_sec = 0;
  stop_delay.tv_nsec = t * 1000 * 1000; // t ms
  clock_nanosleep(CLOCK_MONOTONIC, 0, &stop_delay, NULL);
}
