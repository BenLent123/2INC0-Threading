#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include "arrivals.h"
#include "intersection_time.h"
#include "input.h"

/* 
 * curr_arrivals[][][]
 *
 * A 3D array that stores the arrivals that have occurred
 * The first two indices determine the entry lane: first index is Side, second index is Direction
 * curr_arrivals[s][d] returns an array of all arrivals for the entry lane on side s for direction d,
 *   ordered in the same order as they arrived
 */
static Arrival curr_arrivals[4][4][20];

/*
 * semaphores[][]
 *
 * A 2D array that defines a semaphore for each entry lane,
 *   which are used to signal the corresponding traffic light that a car has arrived
 * The two indices determine the entry lane: first index is Side, second index is Direction
 */
static sem_t semaphores[4][4];

// for manage light threads -> holds argumentss
typedef struct {
    Side side;
    Direction direction;
    //pthread_mutex_t* intersection_mutex;
    pthread_mutex_t** m_exit_lanes;
	pthread_mutex_t** m_squares;
} LightArgs;

/*
 * supply_arrivals()
 *
 * A function for supplying arrivals to the intersection
 * This should be executed by a separate thread
 */
static void* supply_arrivals()
{
  int t = 0;
  int num_curr_arrivals[4][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

  // for every arrival in the list
  for (int i = 0; i < sizeof(input_arrivals)/sizeof(Arrival); i++)
  {
    // get the next arrival in the list
    Arrival arrival = input_arrivals[i];
    // wait until this arrival is supposed to arrive
    sleep(arrival.time - t);
    t = arrival.time;
    // store the new arrival in curr_arrivals
    curr_arrivals[arrival.side][arrival.direction][num_curr_arrivals[arrival.side][arrival.direction]] = arrival;
    num_curr_arrivals[arrival.side][arrival.direction] += 1;
    // increment the semaphore for the traffic light that the arrival is for
    sem_post(&semaphores[arrival.side][arrival.direction]);
  }

  return(0);
}


/*
 * manage_light(void* arg)
 *
 * A function that implements the behaviour of a traffic light
 */
static void* manage_light(void* arg)
{
    LightArgs* args = (LightArgs*)arg;
    Side side = args->side;
    Direction direction = args->direction;
    //pthread_mutex_t* intersection_mutex = args->intersection_mutex;
    
    //Compute direction of exit lane
    
    int exit_lane = (side+direction+1)%4; //This actually works lol. Good thing they coded the directions conviniently.
    //Obviously the exit lane is coded with the same NORTH = 0, EAST = 1, etc as the side
    
    //Extracts only the RELEVANT MUTEXES for each thread
    pthread_mutex_t* m_exit_lanes; 
	pthread_mutex_t* m_squares[2];
	
	m_exit_lanes = args->m_exit_lanes[exit_lane];
		
	
	for(int i=0; i<4; i++){
		m_squares[i] = args->m_squares[i];
		}
	
    int car_index = 0;

    while (get_time_passed() < END_TIME)
    {
        // Wait for a car arrival
        sem_wait(&semaphores[side][direction]);
        
        // Get the arrival information
        Arrival arrival = curr_arrivals[side][direction][car_index];
        car_index++;
        
        
        

        // Lock the intersection mutex
        //pthread_mutex_lock(intersection_mutex);

        

        // Turn the traffic light green
        printf("traffic light %d %d turns green at time %d for car %d\n", side, direction, get_time_passed(), arrival.id);

        // Sleep for CROSS_TIME seconds
        sleep(CROSS_TIME);

        // Turn the traffic light red
        printf("traffic light %d %d turns red at time %d\n", side, direction, get_time_passed());

        // Unlock the intersection mutex
        pthread_mutex_unlock(intersection_mutex);
    }

    // frees memory
    free(args);
    return NULL;
	
  // TODO:
  // while not all arrivals have been handled, repeatedly:
  //  - wait for an arrival using the semaphore for this traffic light
  //  - lock the right mutex(es)
  //  - make the traffic light turn green
  //  - sleep for CROSS_TIME seconds
  //  - make the traffic light turn red
  //  - unlock the right mutex(es)
}


int main(int argc, char * argv[])
{
	//BASIC SOLUTION USES ONLY 1 MUTEX
	//pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
	
	//Advanced solution, 8 mutexes
	pthread_mutex_t m_exit_lanes[4];
	pthread_mutex_t m_squares[4];
	
	//The "square mutexes" are arranged spatially as (by the index you call them)
	//   0 1
	//   2 3
	// 
	// Usage example: {Side:Direction} --> {which mutexes are locked}
	// {South:Straight} ---> {1,3}
	// {South:Left} ---> {0,2}
	// {North:Straight} ---> {0,2}
	// {North:Left} ---> {1,3}
	
	// {East:Straight} ---> {0,1}
	// {East:Left} ---> {2,3}
	// {West:Straight} ---> {0,2}
	// {West:Left} ---> {1,3}
	//Converting to their encoding
	// {2:1} ---> {1,3}
	// {2:0} ---> {0,2}
	// {0:1} ---> {0,2}
	// {0:0} ---> {1,3}
	
	// {2:1} ---> {1,3}
	// {2:0} ---> {0,2}
	// {0:1} ---> {0,2}
	// {0:0} ---> {1,3}

	
	for(int i = 0; i < 4; i++){
		m_exit_lanes[i] = PTHREAD_MUTEX_INITIALIZER;
	}
	
	for(int i = 0; i < 4; i++){
		m_squares[i] = PTHREAD_MUTEX_INITIALIZER;
	}
	
	
	// create semaphores to wait/signal for arrivals
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			sem_init(&semaphores[i][j], 0, 0);
		}
	}

	// start the timer
	start_time();
  
  
	//Creates 16 threads, for now. INVESTIGATE HOW MANHY SHOULD BE MADE
	// Create threads for each traffic light
	pthread_t light_threads[4][4];
	for (int side = 0; side < 4; side++)
	{
      for (int direction = 0; direction < 4; direction++)
      {
          // Allocate memory for arguments
          LightArgs* args = malloc(sizeof(LightArgs));
          if (args == NULL) {
              perror("Failed to allocate memory for thread arguments");
              exit(EXIT_FAILURE);
          }
          args->side = side;
          args->direction = direction;
          args->m_exit_lanes = m_exit_lanes;
          args->m_squares = m_squares;

          pthread_create(&light_threads[side][direction], NULL, manage_light, (void*)args);
      }
  }

  // Create the supply arrivals thread
  pthread_t supplier_thread;
  pthread_create(&supplier_thread, NULL, supply_arrivals, NULL);

	pthread_join(supplier_thread, NULL);

	// Wait until END_TIME
  while (get_time_passed() < END_TIME)
  {
      sleep(1);
  }


  // Nitin used: i: side && j: direction
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
      pthread_cancel(light_threads[i][j]);
			pthread_join(light_threads[i][j], NULL);
			}
	}

  // destroy semaphores
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      sem_destroy(&semaphores[i][j]);
    }
  }

  return 0;
}


//////////////////////////////////////////

//			||||
//			<v>^ o
//o		
//-<					^-
//-^					<-
//->					v-
//-v					>-
//						o	
//		 o	v<^>
//			||||

//rules:
//U-turns: only block their exit lane --> 1 mutex per exit lane
//rights: only blocks exit lane

// The intersection can support right and u-turns however (they will block exit lanes though)
// The intersection can only support the following situations (regarding the straight and left turns)
//		1. Only 1 car in the intersection turning straight or left
// 		2. One car comes from the north, the other from the south and BOTH turn straight
// 		3. One car comes from the north, the other from the south and BOTH turn left
//		4. Situation 2 or 3 but with "north" replaced by "east" and "south" replaced by "west"



//straights: block adjacent straights (but not the opposite straight) + blocks all left turns (except from its own direction)

//lefts: for leftward direction blocks all non u-turns + for rightward direction blocks left-turns + for forward direct, blocks right turns
//lefts (rephrased): for leftward direction blocks all turns except right turn + blocks access to own direction's exit lane (except from leftward direction). 
//lefts (rephrased again): for leftward direction blocks all turns except right turn + blocks left turns from adjacent directions but not from opposite direction. 


