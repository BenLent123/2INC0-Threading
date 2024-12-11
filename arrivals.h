#ifndef ARRIVALS_H
#define ARRIVALS_H

// the side of an intersection
typedef enum {NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3} Side;

// the direction that a car can go in over the intersection
typedef enum {LEFT = 0, STRAIGHT = 1, RIGHT = 2, UTURN = 3} Direction;

// a data structure to represent the arrival of a car
typedef struct
{
  int id;                 // the id of the car
  Side side;              // the side of the intersection at whcih the car arrives
  Direction direction;    // the direction the car wants to go
  int time;               // the time in seconds at which the car arrives
} Arrival;

#endif
