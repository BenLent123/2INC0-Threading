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

static Arrival curr_arrivals[4][4][20];
static sem_t semaphores[4][4];

typedef struct {
    Side side;
    Direction direction;
    pthread_mutex_t** m_exit_lanes;
    pthread_mutex_t** m_squares;
} LightArgs;

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


static void* manage_light(void* arg)
{
    LightArgs* args = (LightArgs*)arg;
    Side side = args->side;
    Direction direction = args->direction;
    int exit_lane = (side+direction+1)%4; //This actually works lol. Good thing they coded the directions conviniently. //Obviously the exit lane is coded with the same NORTH = 0, EAST = 1, etc as the side. //Extracts only the RELEVANT MUTEXES for each thread
    pthread_mutex_t* m_exit_lanes; 
    pthread_mutex_t* m_squares_0;
    pthread_mutex_t* m_squares_1;
    m_exit_lanes = args->m_exit_lanes[exit_lane];
  
	if((direction==0)||(direction==1)){//square mutex only needed for left and straight turns	
		int selector = side*10 + direction;
		  switch(selector){
			  case 21: 
				  m_squares_0 = args->m_squares[1];
				  m_squares_1 = args->m_squares[3];
			  break;
			  case 20: 
				  m_squares_0 = args->m_squares[0];
				  m_squares_1 = args->m_squares[2];
			  break;
			  case 1: 
				  m_squares_0 = args->m_squares[0];
				  m_squares_1 = args->m_squares[2];
			  break;
			  case 0: 
				  m_squares_0 = args->m_squares[1];
				  m_squares_1 = args->m_squares[3];
			  break;
			  case 11: 
				  m_squares_0 = args->m_squares[0];
				  m_squares_1 = args->m_squares[1];
			  break;
			  case 10: 
				  m_squares_0 = args->m_squares[2];
				  m_squares_1 = args->m_squares[3];
			  break;
			  case 31: 
				  m_squares_0 = args->m_squares[2];
				  m_squares_1 = args->m_squares[3];
			  break;
			  case 30: 
				  m_squares_0 = args->m_squares[0];
				  m_squares_1 = args->m_squares[1];
			  break;
			}
	}
	
    int car_index = 0;
    while (get_time_passed() < END_TIME)
    {
        // Wait for a car arrival
        sem_wait(&semaphores[side][direction]);
        
        // Get the arrival information
        Arrival arrival = curr_arrivals[side][direction][car_index];
        car_index++;
        
        //lock the mutexes
        pthread_mutex_lock(m_exit_lanes);
        perror("Lane %d locked",exit_lane);
        if((direction==0)||(direction==1)){
			pthread_mutex_lock(m_squares_0);
			perror("Square 0 locked");
			pthread_mutex_lock(m_squares_1);
			perror("Square 1 locked");
		}
        // Turn the traffic light green
        printf("traffic light %d %d turns green at time %d for car %d\n", side, direction, get_time_passed(), arrival.id);

        // Sleep for CROSS_TIME seconds
        sleep(CROSS_TIME);

        // Turn the traffic light red
        printf("traffic light %d %d turns red at time %d\n", side, direction, get_time_passed());

        // Unlock the intersection mutex
        //pthread_mutex_unlock(intersection_mutex);
        pthread_mutex_unlock(m_exit_lanes);
        perror("Lane %d unlocked",exit_lane);
        if((direction==0)||(direction==1)){
			pthread_mutex_unlock(m_squares_0);
			perror("Square 0 unlocked");
			pthread_mutex_unlock(m_squares_1);
			perror("Square 1 unlocked");
		}
    }

    // frees memory
    free(args);
    return NULL;

}


int main(int argc, char * argv[])
{
	
	//Advanced solution, 8 mutexes
	pthread_mutex_t* mp_exit_lanes[4];
	pthread_mutex_t* mp_squares[4];
	pthread_mutex_t m_exit_lanes[4];
	pthread_mutex_t m_squares[4];
	
	
	for(int i = 0; i < 4; i++){
		//m_exit_lanes[i] = PTHREAD_MUTEX_INITIALIZER;
		pthread_mutex_init(&m_exit_lanes[i], NULL);
		mp_exit_lanes[i] = &m_exit_lanes[i];
	}
	
	for(int i = 0; i < 4; i++){
		//m_squares[i] = PTHREAD_MUTEX_INITIALIZER;
		pthread_mutex_init(&m_squares[i], NULL);
		mp_squares[i] = &m_squares[i];
	}
	
	// create semaphores to wait/signal for arrivals
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			sem_init(&semaphores[i][j], 0, 0);
		}
	}

	// start the timer
	start_time();
  
	// Create threads for each traffic light
	pthread_t light_threads[4][4];
	for (int side = 0; side < 4; side++){
		for (int direction = 0; direction < 4; direction++){
			// Allocate memory for arguments
			LightArgs* args = malloc(sizeof(LightArgs));
			if (args == NULL) {
				perror("Failed to allocate memory for thread arguments");
				exit(EXIT_FAILURE);
			}
			args->side = side;
			args->direction = direction;
			args->m_exit_lanes = mp_exit_lanes;
			args->m_squares = mp_squares;

			pthread_create(&light_threads[side][direction], NULL, manage_light, (void*)args);
		}
	}

	// Create the supply arrivals thread
	pthread_t supplier_thread;
	pthread_create(&supplier_thread, NULL, supply_arrivals, NULL);
	
	//Block on waiting for supplier thread to terminate
	pthread_join(supplier_thread, NULL);

	// Wait until END_TIME
	while (get_time_passed() < END_TIME){
		sleep(1);
	}

	// notation used i: side && j: direction
	for (int side = 0; side < 4; side++){
		for (int direction = 0; direction < 4; direction++){
			pthread_cancel(light_threads[side][direction]);
			pthread_join(light_threads[side][direction], NULL);
		}
	}

	// destroy semaphores
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			sem_destroy(&semaphores[i][j]);
		}
	}
	
	//Destroy mutexes
	for(int i = 0; i < 4; i++){
		pthread_mutex_destroy(&m_exit_lanes[i]);
	}
	
	for(int i = 0; i < 4; i++){
		pthread_mutex_destroy(&m_squares[i]);
	}

  return 0;
}


