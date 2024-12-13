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

  int* args = (int*)arg;
	//extract args from arg pointer
	int i = args[0];
	int j = args[1];
	int k = 0;
  pthread_mutex_t* m = (pthread_mutex_t*)args[2];
	
	while(1){
		//lane information is passed around as i,j
		//Extract lane information. Access need not be mutxed
		Arrival curr_arrival = curr_arrivals[i][j][k];
	
		//Wait on semaphore, which wraps an atomic expression
		sem_wait(&semaphores[i][j]);
		while (pthread_mutex_lock(&m)) { /* an error has occurred */
			perror("pthread_mutex_lock");
		}
		//make traffic light turn green
		
		sleep(CROSS_TIME);
		
		while (pthread_mutex_unlock(&m)) { /* an error has occurred */
			perror("pthread_mutex_unlock");
		}
	}
	pthread_exit(NULL);
	
  // TODO:
  // while not all arrivals have been handled, repeatedly:
  //  - wait for an arrival using the semaphore for this traffic light
  //  - lock the right mutex(es)
  //  - make the traffic light turn green
  //  - sleep for CROSS_TIME seconds
  //  - make the traffic light turn red
  //  - unlock the right mutex(es)

  return(0);
}


int main(int argc, char * argv[])
{
	//BASIC SOLUTION USES ONLY 1 MUTEX
	pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
	
	
	// create semaphores to wait/signal for arrivals
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			sem_init(&semaphores[i][j], 0, 0);
		}
	}

	// start the timer
	start_time();
  
  
	//Creates 16 threads, for now. INVESTIGATE HOW MANHY SHOULD BE MADE
    int        thr_id[4][4];         /* thread ID for the newly created thread */
    pthread_t  p_thread[4][4];       /* thread's structure                     */
	pthread_t  supply_arrivals_thread;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			/* create a new thread that will execute 'do_loop()' */
			int args[3] = {i, j, (int)&m};
			thr_id[i][j] = pthread_create(&p_thread[i][j], NULL, manage_light, (void*)args);
			}
	}

	int thread_id = pthread_create(&supply_arrivals_thread, NULL, supply_arrivals, NULL);
	// TODO: create a thread that executes supply_arrivals

	// TODO: wait for all threads to finish
	pthread_join(&supply_arrivals_thread, NULL);
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			/* create a new thread that will execute 'do_loop()' */
			pthread_join(&p_thread[i][j], NULL);
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
}
