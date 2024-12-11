#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif

#ifndef TIMER_ABSTIME
#define TIMER_ABSTIME 1
#endif

#ifndef INTERSECTION_TIME_H
#define INTERSECTION_TIME_H

/*
 * start_time()
 *
 * store the current time to use as the starting time of the simulation of the intersection
 */
void start_time();

/*
 * sleep_until_arrival(int timestamp)
 *
 * sleep until the timestamp happens, using the starting time as base
 * should only be used by supply_arrivals
 */
void sleep_until_arrival(int timestamp);

/*
 * get_time_passed()
 *
 * get the time in seconds that passed since the starting time
 */
int get_time_passed();

#endif
