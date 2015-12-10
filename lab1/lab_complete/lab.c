#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>

// This file contains the definition of MAX_ITERATIONS, among other things.
#include "samples.h"


// Delay in nanoseconds (1 millisecond)
#define DELAY 1000000

// Number of samples that do_work() handles
#define PROCESSING_INTERVAL  256 

// Could be a local variable, but you may need it as a static variable
// here when you modify this file according to the lab instructions.
static int sample_buffer[PROCESSING_INTERVAL];
static int work_buffer[PROCESSING_INTERVAL];

static int done = 0;

//Mutex def
pthread_mutex_t mutex;

//Semaphore def
sem_t start_work; 

static void copy_buffer(void){
	int i;
	pthread_mutex_lock(&mutex);
	for(i=0; i<PROCESSING_INTERVAL; i++){
		work_buffer[i]=sample_buffer[i];
	}
	pthread_mutex_unlock(&mutex);
}

static void cont_work(int *cont){
	pthread_mutex_lock(&mutex);
	*cont = !done;
	pthread_mutex_unlock(&mutex);
}

static void sample_done(void){
	pthread_mutex_lock(&mutex);
	done = 1;
	pthread_mutex_unlock(&mutex);
}

void do_work(int *samples)
{
        int i;

        //  A busy loop. (In a real system this would do something
        //  more interesting such as an FFT or a particle filter,
        //  etc...)
        volatile int dummy; // A compiler warning is ok here
        for(i=0; i < 20000000;i++){
                dummy=i;
        }

        // Write out the samples.
        for(i=0; i < PROCESSING_INTERVAL; i++){
                write_sample(0,samples[i]);
        }

}

struct timespec firsttime;
void *sampletask(void *arg)
{

	// Set our thread to real time priority
	struct sched_param sp;
	sp.sched_priority = 30;
	if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp)){
		fprintf(stderr,"WARNING: Failed to set stepper thread"
			"to real-time priority\n");
	}

        int channel = 0;
        struct timespec current;
        int i;
        int samplectr = 0;
        current = firsttime;

        for(i=0; i < MAX_ITERATIONS; i++){
                clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &current, NULL);


                sample_buffer[samplectr] = read_sample(channel);
                samplectr++;
                if(samplectr == PROCESSING_INTERVAL){
                        samplectr = 0;
			sem_post(&start_work);
                }

                // Increment current time point
                current.tv_nsec +=  DELAY;
                if(current.tv_nsec >= 1000000000){
                        current.tv_nsec -= 1000000000;
                        current.tv_sec++;
                }


        }

	sample_done();
	sem_post(&start_work);

        return NULL;
}

void *doworktask(void *arg)
{
	// Set our thread to real time priority
	struct sched_param sp;
	sp.sched_priority = 15;
	if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp)){
		fprintf(stderr,"WARNING: Failed to set stepper thread"
			"to real-time priority\n");
	}
	

	int cont = 1;
	while(cont){
		sem_wait(&start_work);
		copy_buffer();	
	       	do_work(work_buffer);
		cont_work(&cont);
	
	}
	return NULL;
}


int main(int argc,char **argv)
{
        pthread_t thread0;
	pthread_t thread1;
        pthread_attr_t attr;


	//Mutex and semaphore
	pthread_mutex_init(&mutex, NULL);
	sem_init(&start_work, 0, 0);

	// Lock memory to ensure no swapping is done.
        if(mlockall(MCL_FUTURE|MCL_CURRENT)){
                fprintf(stderr,"WARNING: Failed to lock memory\n");
        }

        clock_gettime(CLOCK_MONOTONIC, &firsttime);

        // Start the sampling at an even multiple of a second (to make
        // the sample times easy to analyze by hand if necessary)
        firsttime.tv_sec+=2;
        firsttime.tv_nsec = 0;
        printf("Starting sampling at about t+2 seconds\n");
        
        samples_init(&firsttime);

        if(pthread_attr_init(&attr)){
                perror("pthread_attr_init");
        }
        // Set default stacksize to 64 KiB (should be plenty)
        if(pthread_attr_setstacksize(&attr, 65536)){
                perror("pthread_attr_setstacksize()");
        }
        
        pthread_create(&thread0, &attr, sampletask, NULL);
	pthread_create(&thread1, &attr, doworktask, NULL); 

        pthread_join(thread0, NULL);
        pthread_join(thread1, NULL);

        // Dump output data which will be used by the analyze.m script
        dump_outdata();
        dump_sample_times();
        return 0;
}