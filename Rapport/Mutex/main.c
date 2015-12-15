#include <sys/time.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_N_ITERATIONS 100000
#define MAX_N_INT 10

struct timeval starttime;
struct timeval endtime;



pthread_mutex_t mutex;
sem_t sem0;
sem_t sem1; 

volatile int dummy[MAX_N_INT];
volatile int super_dummy;

long long int record_time_sem;
long long int start_time_sem;

long long int record_time_mutex_lock;
long long int record_time_mutex_unlock;


static void *thread0(void *idptr)
{
	int iter=0;
	int i;
	int timediff = 0;
	while(iter<MAX_N_ITERATIONS){
		sem_wait(&sem0);

		gettimeofday(&starttime, NULL);
		pthread_mutex_lock(&mutex);
		
		gettimeofday(&endtime, NULL);
		timediff = (endtime.tv_sec*1000000ULL + endtime.tv_usec) -
			(starttime.tv_sec*1000000ULL + starttime.tv_usec);
               	record_time_mutex_lock=record_time_mutex_lock+timediff;

		for(i=0; i<MAX_N_INT; i++){
			if(dummy[i]){
				super_dummy++;
			}
		}
		iter++;

		gettimeofday(&starttime, NULL);
		pthread_mutex_unlock(&mutex);

		gettimeofday(&endtime, NULL);
		timediff = (endtime.tv_sec*1000000ULL + endtime.tv_usec) -
			(starttime.tv_sec*1000000ULL + starttime.tv_usec);
               	record_time_mutex_unlock=record_time_mutex_unlock+timediff;



		

		gettimeofday(&starttime, NULL);
		sem_post(&sem1);
		
	}

	return NULL;
}

static void *thread1(void *idptr)
{
	
	int iter=0;
	int i;
	long long int timediff = 0;
	sem_post(&sem0);
	while(iter<MAX_N_ITERATIONS){
		sem_wait(&sem1);

		gettimeofday(&endtime, NULL);
		timediff = (endtime.tv_sec*1000000ULL + endtime.tv_usec) -
			(starttime.tv_sec*1000000ULL + starttime.tv_usec);
               	record_time_sem=record_time_sem+timediff;

		
		pthread_mutex_lock(&mutex);
		for(i=0; i<MAX_N_INT; i++){
			if(dummy[i]){
				super_dummy--;
			}
		}
		iter++;
		pthread_mutex_unlock(&mutex);
		sem_post(&sem0);
	}

	return NULL;
}







int main(int argc, char **argv)
{
	sem_init(&sem0, 0, 0);
	sem_init(&sem1, 0, 0);
	pthread_mutex_init(&mutex,NULL);

	int i;

	for(i=0; i<MAX_N_INT; i++){
		dummy[i]=1;
	}



	pthread_t thread0_handle;
	pthread_t thread1_handle;

	pthread_create(&thread0_handle, NULL, thread0, 0);
	pthread_create(&thread1_handle, NULL, thread1, 0);

	pthread_join(thread0_handle, NULL);
	pthread_join(thread1_handle, NULL);



	fprintf(stderr,"Sem time: %lld \n",record_time_sem);
	fprintf(stderr,"Mutex lock time: %lld \n",record_time_mutex_lock);
	fprintf(stderr,"Mutex unlock time: %lld \n",record_time_mutex_unlock);
	fprintf(stderr,"Mutex tot time: %lld \n",record_time_mutex_unlock+record_time_mutex_lock);

	FILE *fp = fopen("output.txt","w");
	fprintf(stderr, "Dumping outdata\n");
	
	fprintf(fp, "%lld ", record_time_sem);
	fprintf(fp, "\n");		
	fprintf(fp, "%lld ", record_time_mutex_lock);
	fprintf(fp, "\n");
	fprintf(fp, "%lld ", record_time_mutex_unlock);		
	fprintf(fp, "\n");
	fprintf(fp, "%lld ", record_time_mutex_unlock+record_time_mutex_lock);
	fprintf(fp, "\n");	
	fclose(fp);	

	return 0;
}
