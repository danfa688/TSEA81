#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "lift.h"
#include "si_ui.h"
#include "debug.h"
#include <sys/time.h>


// Unfortunately the rand() function is not thread-safe. However, the
// rand_r() function is thread-safe, but need a pointer to an int to
// store the current state of the pseudo-random generator.  This
// pointer needs to be unique for every thread that might call
// rand_r() simultaneously. The functions below are wrappers around
// rand_r() which should work in the environment encountered in
// assignment 3.
//
int done[MAX_N_PERSONS];
int outdata[MAX_N_PERSONS];


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

static int all_done(void){
	int i;
	pthread_mutex_lock(&Lift->mutex);
	for(i = 0; i < MAX_N_PERSONS; i++){
		if(!done[i]){
		pthread_mutex_unlock(&Lift->mutex);
			return 0;
		}
	}
	pthread_mutex_unlock(&Lift->mutex);	
	return 1;
}

static void dump_outdata(void)
{
	FILE *fp = fopen("output.txt","w");
	int i;
	fprintf(stderr, "Dumping outdata\n");

	pthread_mutex_lock(&Lift->mutex);
	for(i=0; i < MAX_N_PERSONS; i++){
		fprintf(fp, "%d ", outdata[i]);
		fprintf(fp, "\n");
	}
	pthread_mutex_unlock(&Lift->mutex);
	fclose(fp);
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
	sem_post(&(Lift->sem));

	int iterations = 0;
	struct timeval starttime;
    	struct timeval endtime;
    	long long int timediff = 0;
	long long int total_time = 0;
	

	while(1){
		gettimeofday(&starttime, NULL);


		// * Select random floors
		// * Travel between these floors
		// * Wait a little while
		int from_floor = get_random_value(id, N_FLOORS-1);
		int to_floor = get_random_value(id, N_FLOORS-1);
		while(from_floor == to_floor){
			to_floor = get_random_value(id, N_FLOORS-1);
		}

		lift_travel(Lift, id, from_floor, to_floor);


		gettimeofday(&endtime, NULL);
    		timediff = (endtime.tv_sec*1000000ULL + endtime.tv_usec) -
               		(starttime.tv_sec*1000000ULL + starttime.tv_usec);
		total_time = total_time + timediff;
		if(iterations == MAX_ITERATIONS){
			printf("ID %d done! Took %lld us.\n", id, total_time);
			pthread_mutex_lock(&Lift->mutex);
			done[id] = 1;
			outdata[id] = total_time;
			pthread_mutex_unlock(&Lift->mutex);
			sem_post(&(Lift->sem_done));
		}
		iterations++;

	}
	return NULL;
}

static void *user_thread(void *unused)
{
	int current_passenger_id = 0;
	pthread_t passenger_thread_handle[MAX_N_PERSONS];

	for(current_passenger_id = 0; current_passenger_id < MAX_N_PERSONS; current_passenger_id++){
		pthread_create(&passenger_thread_handle[current_passenger_id],NULL, passenger_thread,(void *) &current_passenger_id);
		sem_wait(&(Lift->sem));
	}
	
	while(1){
		sem_wait(&(Lift->sem_done));	
		if(all_done()){
			dump_outdata();
			lift_delete(Lift);
			int i;
			for(i=0; i < current_passenger_id; i++){
				//pthread_join(passenger_thread_handle[i], NULL); //Why is this!!!
			}
			exit(0);
		}
	}

	return NULL;
}

int main(int argc, char **argv)
{
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
