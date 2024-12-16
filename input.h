#ifndef INPUT_H
#define INPUT_H

#include "arrivals.h"

// the time in seconds it takes for a car to cross the intersection
#define CROSS_TIME 5

// the time in seconds the traffic lights should be alive
#define END_TIME 40

// the array of arrivals for the intersection
const Arrival input_arrivals[] = {{0, 1, 0, 0},{1, 1, 1, 1},{2, 1, 2, 2},{3, 2, 0, 3},{4, 2, 1, 4},{5, 2, 2, 5},{6, 2, 3, 6},{7, 3, 0, 7},{8, 3, 1, 8},{9, 3, 2, 9}};

#endif
