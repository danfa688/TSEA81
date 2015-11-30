#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "lift.h"
#include "si_ui.h"
#include "debug.h"


// Unfortunately the rand() function is not thread-safe. However, the
// rand_r() function is thread-safe, but need a pointer to an int to
// store the current state of the pseudo-random generator.  This
// pointer needs to be unique for every thread that might call
// rand_r() simultaneously. The functions below are wrappers around
// rand_r() which should work in the environment encountered in
// assignment 3.
//

static unsigned int rand_r_state[MAX_N_PERSONS];
// Get a random value between 0 and maximum_value. The passenger_id
// parameter is used to ensure that the rand_r() function is called in
// a thread-safe manner.

static int get_random_value(int passenger_id, int maximum_value)
{
	return rand_r(&rand_r_state[passenger_id]) % (maximum_value + 1);
}

static lift_type Lift;

// Initialize the random seeds used by the get_random_value() function
// above.
static void init_random(void)
{
	int i;
	for(i=0; i < MAX_N_PERSONS; i++){
		// Use this statement if the same random number sequence
		// shall be used for every execution of the program.
		rand_r_state[i] = i;

		// Use this statement if the random number sequence
		// shall differ between different runs of the
		// program. (As the current time (as measured in
		// seconds) is used as a seed.)
		rand_r_state[i] = i+time(NULL);
	}
}



static void *lift_thread(void *unused)
{
	int next_floor=0;
	int change_direction=0;
	while(1){
		// Move lift one floor
		lift_next_floor(Lift, &next_floor, &change_direction);
		lift_move(Lift, next_floor, change_direction);
		lift_has_arrived(Lift);
		//usleep(3000000);
	}
	return NULL;
}

static void *passenger_thread(void *idptr)
{
	// Code that reads the passenger ID from the idptr pointer
	// (due to the way pthread_create works we need to first cast
	// the void pointer to an int pointer).

	int *tmp = (int *) idptr;
	int id = *tmp;

	while(1){
		// * Select random floors
		// * Travel between these floors
		// * Wait a little while
		int from_floor = get_random_value(id, N_FLOORS-1);
		int to_floor = get_random_value(id, N_FLOORS-1);

		debug_check_override(id, &from_floor, &to_floor);

		lift_travel(Lift, id, from_floor, to_floor);
		usleep(5000000);
	}
	return NULL;
}

static void *user_thread(void *unused)
{
	int current_passenger_id = 0;
	char message[SI_UI_MAX_MESSAGE_SIZE]; 
	pthread_t passenger_thread_handle[MAX_N_PERSONS];
	si_ui_set_size(670, 700); 
	debug_init();
	
	while(1){
		// Read a message from the GUI
		si_ui_receive(message);
		if(!strcmp(message, "new")){
			// create a new passenger if possible, else
			// use si_ui_show_error() to show an error
			// message if too many passengers have been
			// created. Make sure that each passenger gets
			// a unique ID between 0 and MAX_N_PERSONS-1.
			int temp_passenger_id = current_passenger_id;
			if(temp_passenger_id < MAX_N_PERSONS){
				pthread_create(&passenger_thread_handle[temp_passenger_id],NULL, passenger_thread,(void *) &temp_passenger_id);
				current_passenger_id++;
			}
			else{
				si_ui_show_error("too many passengers have been created");
			}
			
		}else if(!strcmp(message, "exit")){
			lift_delete(Lift);
			int i;
			for(i=0; i < current_passenger_id; i++){
				pthread_join(passenger_thread_handle[i], NULL);
			}
			exit(0);
		}else if(!strcmp(message, "pause")){
			debug_pause();
		}else if(!strcmp(message, "unpause")){
			debug_unpause();
		}else if(!strcmp(message, "test")){

			debug_override(0, 0, 4);
			debug_override(1, 0, 4); 
			debug_override(2, 0, 4); 
			debug_override(3, 0, 4); 
			debug_override(4, 0, 4); 
			debug_override(5, 2, 4);   

		}else {
            		si_ui_show_error("unexpected message type"); 
        	}
	}

	return NULL;
}

int main(int argc, char **argv)
{
	si_ui_init();
	init_random();
	Lift = lift_create();


        // Create tasks as appropriate here
	pthread_t user_thread_handle;
	pthread_t lift_thread_handle;
	
	pthread_create(&user_thread_handle, NULL, user_thread, 0);
	pthread_create(&lift_thread_handle, NULL, lift_thread, 0);
	
	pthread_join(user_thread_handle, NULL);
	pthread_join(lift_thread_handle, NULL);
	

	return 0;
}
