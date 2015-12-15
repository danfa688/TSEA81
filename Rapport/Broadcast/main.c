#include <sys/time.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_N_THREADS 1
#define MAX_N_ITERATIONS 10000

sem_t sem;
pthread_mutex_t mutex; 
pthread_cond_t broadcast;

long long int record_time[MAX_N_THREADS];
long long int start_time[MAX_N_THREADS];

int next_iter(void){
	int i;
	pthread_mutex_lock(&mutex);
	for(i=0; i<MAX_N_THREADS;i++){
		if(start_time[i] != 0){
			pthread_mutex_unlock(&mutex);
			return 0;
		}
	}	

	pthread_mutex_unlock(&mutex);
	return 1;
}

static void dump_outdata(void){
	FILE *fp = fopen("output.txt","w");
	int i;
	fprintf(stderr, "Dumping outdata\n");
	
	for(i=0; i < MAX_N_THREADS; i++){
		fprintf(fp, "%lld ", record_time[i]);
		fprintf(fp, "\n");
	}
	fclose(fp);
}


static void *extra_thread(void *idptr)
{
	int *tmp = (int *) idptr;
	int id = *tmp;
	sem_post(&sem);
    	struct timeval endtime;
	long long int timediff;

	while(1){
		pthread_mutex_lock(&mutex);
		while(start_time[id] == 0){
			pthread_cond_wait(&broadcast,&mutex);
		}
		gettimeofday(&endtime, NULL);
		timediff = (endtime.tv_sec*1000000ULL + endtime.tv_usec) - start_time[id];

		record_time[id]=record_time[id]+timediff;
		start_time[id] = 0;
		pthread_mutex_unlock(&mutex);
	}

	return NULL;
}


static void *main_thread(void *unused)
{
	pthread_t thread_handle[MAX_N_THREADS];
	int thread_id;
	struct timeval starttime;

	int iter = 0;
	int i;


	for(thread_id = 0; thread_id < MAX_N_THREADS; thread_id++){
		pthread_create(&thread_handle[thread_id],NULL, extra_thread,(void *) &thread_id);
		sem_wait(&sem);
	}
	
	while(iter < MAX_N_ITERATIONS){
		if(next_iter()){
			int i;
			gettimeofday(&starttime, NULL);
			for(i=0;i<MAX_N_THREADS;i++){
				start_time[i]=(starttime.tv_sec*1000000ULL + 		starttime.tv_usec);				
			}
			pthread_mutex_lock(&mutex);
			pthread_cond_broadcast(&broadcast);
			pthread_mutex_unlock(&mutex);
			iter++;
		}
		else{
			fprintf(stderr, "Not all broadcast are done!\n");
		}
		usleep(3000);
	}
	

	for(i=0; i < thread_id; i++){
		//pthread_join(thread_handle[i], NULL); //Why is this??
	}
	
	pthread_mutex_lock(&mutex);
	for(i=0; i < MAX_N_THREADS; i++){
		fprintf(stderr, "Thread %d, done on time: %lld \n", i, record_time[i]);
	}
	dump_outdata();
	pthread_mutex_unlock(&mutex);


	return NULL;
}




int main(int argc, char **argv)
{

	sem_init(&sem, 0, 0);
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&broadcast,NULL); 

	pthread_t main_thread_handle;

	pthread_create(&main_thread_handle, NULL, main_thread, 0);

	pthread_join(main_thread_handle, NULL);

	return 0;
}
