#ifndef INPUT_H
#define INPUT_H

#include "arrivals.h"

// the time in seconds it takes for a car to cross the intersection
#define CROSS_TIME 5

// the time in seconds the traffic lights should be alive
#define END_TIME 40

// the array of arrivals for the intersection
const Arrival input_arrivals[] = {{0, EAST, STRAIGHT, 0}, {1, WEST, LEFT, 1}, {2, SOUTH, STRAIGHT, 7}, {3, SOUTH, UTURN, 13}};

#endif
