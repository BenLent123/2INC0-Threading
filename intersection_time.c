#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "intersection_time.h"


struct timespec begin_time;


void start_time()
{
  clock_gettime(CLOCK_REALTIME, &begin_time);
}


void sleep_until_arrival(int timestamp)
{
  struct timespec wait_time = begin_time;
  wait_time.tv_sec += timestamp;
  clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &wait_time, NULL);
}


int get_time_passed()
{
  struct timespec new_time;
  clock_gettime(CLOCK_REALTIME, &new_time);
  return (int)new_time.tv_sec - begin_time.tv_sec;
}
