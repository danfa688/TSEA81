/* A clock program 

Two tasks are used, clock_task for updating a clock, and 
set_task for setting the clock 

*/ 

/* standard library includes */ 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "display.h"
#include "clock.h"


#include "si_ui.h"

/* time data type */ 
typedef struct 
{
    int hours; 
    int minutes; 
    int seconds; 
} time_data_type;


/* clock data type */ 
typedef struct
{
    /* the current time */ 
    time_data_type time; 
    /* alarm time */ 
    time_data_type alarm_time; 
    /* alarm enabled flag */ 
    int alarm_enabled; 

    /* semaphore for mutual exclusion */ 
    pthread_mutex_t mutex; 

    /* semaphore for alarm activation */ 
    sem_t start_alarm; 

} clock_data_type; 

/* the actual clock */ 
static clock_data_type Clock; 

/* functions operating on the clock */ 

/* clock_init: initialise clock */ 
void clock_init(void)
{
    /* initialise time and alarm time */ 

    Clock.time.hours = 0; 
    Clock.time.minutes = 0; 
    Clock.time.seconds = 0; 

    Clock.alarm_time.hours = 0; 
    Clock.alarm_time.minutes = 0; 
    Clock.alarm_time.seconds = 0; 
    
    /* alarm is not enabled */ 
    Clock.alarm_enabled = 0; 

    /* initialise semaphores */ 
    pthread_mutex_init(&Clock.mutex, NULL); 
    sem_init(&Clock.start_alarm, 0, 0); 
}

/* clock_set_time: set current time to hours, minutes and seconds */ 
void clock_set_time(int hours, int minutes, int seconds)
{
    pthread_mutex_lock(&Clock.mutex); 

    Clock.time.hours = hours; 
    Clock.time.minutes = minutes; 
    Clock.time.seconds = seconds; 

    pthread_mutex_unlock(&Clock.mutex); 
}

/* clock_set_alarm: set alarm time to hours, minutes and seconds */ 
void clock_set_alarm(int hours, int minutes, int seconds)
{
    pthread_mutex_lock(&Clock.mutex); 

    Clock.alarm_time.hours = hours; 
    Clock.alarm_time.minutes = minutes; 
    Clock.alarm_time.seconds = seconds;
    Clock.alarm_enabled = 1;
    
    pthread_mutex_unlock(&Clock.mutex);
    display_alarm_time(hours, minutes, seconds);
}

/* clock_alarm_enabled: is alarm enabled? */
int clock_alarm_enabled(void)
{
    int tmp;
    pthread_mutex_lock(&Clock.mutex);
    tmp = Clock.alarm_enabled;
    pthread_mutex_unlock(&Clock.mutex);
    return tmp;
}

/* clock_reset_alarm: reset alarm*/
void clock_reset_alarm(void)
{
    pthread_mutex_lock(&Clock.mutex);
    Clock.alarm_enabled = 0;
    pthread_mutex_unlock(&Clock.mutex);
    erase_alarm_time();
    erase_alarm_text();
}


/* clock_get_time: read time from clock*/ 
void clock_get_time(int *hours, int *minutes, int *seconds)
{
    pthread_mutex_lock(&Clock.mutex);
 
    /* read values */ 
    *hours = Clock.time.hours; 
    *minutes = Clock.time.minutes; 
    *seconds = Clock.time.seconds;

    pthread_mutex_unlock(&Clock.mutex); 
}

/* clock_get_alarm_time: read time from alarm*/ 
void clock_get_alarm_time(int *hours, int *minutes, int *seconds)
{
    pthread_mutex_lock(&Clock.mutex);
 
    /* read values */ 
    *hours = Clock.alarm_time.hours; 
    *minutes = Clock.alarm_time.minutes; 
    *seconds = Clock.alarm_time.seconds;

    pthread_mutex_unlock(&Clock.mutex); 
}

/* clock_check_alarm: check if alarm time is reached */
void clock_check_and_sound_alarm(void)
{
    int hours, minutes, seconds;
    int alarm_hours, alarm_minutes, alarm_seconds;
    clock_get_time(&hours, &minutes, &seconds);
    clock_get_alarm_time(&alarm_hours, &alarm_minutes, &alarm_seconds);
    pthread_mutex_lock(&Clock.mutex);
    int alarm_enabled = Clock.alarm_enabled;
    pthread_mutex_unlock(&Clock.mutex);
    if(alarm_enabled == 1 &&hours == alarm_hours && minutes == alarm_minutes && seconds == alarm_seconds)
    {
        sem_post(&Clock.start_alarm);
    }
    
}

/* increment_time: increments the current time with 
   one second */ 
void increment_time(void)
{
//Local time variables
int Hours;
int Minutes;
int Seconds;

clock_get_time(&Hours, &Minutes, &Seconds);

    /* increment time */ 
    Seconds++; 
    if (Seconds > 59)
    {
        Seconds = 0; 
        Minutes++; 
        if (Minutes > 59)
        {
            Minutes = 0; 
            Hours++; 
            if (Hours > 23)
            {
                Hours = 0; 
            }
        }
    }

clock_set_time(Hours, Minutes, Seconds);
}


/* time_from_set_message: extract time from set message from user interface */ 
void time_from_set_message(char message[], int *hours, int *minutes, int *seconds)
{
    sscanf(message,"set %d %d %d",hours, minutes, seconds); 
}

/* time_from_alarm_message: extract time from alarm message from user interface */ 
void time_from_alarm_message(char message[], int *hours, int *minutes, int *seconds)
{
    sscanf(message,"alarm %d %d %d",hours, minutes, seconds); 
}

/* time_ok: returns nonzero if hours, minutes and seconds represents a valid time */ 
int time_ok(int hours, int minutes, int seconds)
{
    return hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59 && 
        seconds >= 0 && seconds <= 59; 
}



/* tasks */ 

/* clock_task: clock task */ 
void *clock_thread(void *unused) 
{
    /* local copies of the current time */ 
    int hours, minutes, seconds;

    /* infinite loop */ 
    while (1)
    {
        /* read and display current time */ 
        clock_get_time(&hours, &minutes, &seconds); 
        display_time(hours, minutes, seconds);
        
        /* check alarm */
        clock_check_and_sound_alarm();

        /* increment time */ 
        increment_time(); 

        /* wait one second */ 
        usleep(1000000);
    }
}

/*alarm_task: handles the alarm*/
void *alarm_thread(void *unused)
{
    /* local copies of the current time */ 
    int hours, minutes, seconds; 
    
    while(1)
    {
        sem_wait(&Clock.start_alarm);
        display_alarm_text();
        usleep(1500000);
        pthread_mutex_lock(&Clock.mutex);
        int alarm_enabled = Clock.alarm_enabled;
        pthread_mutex_unlock(&Clock.mutex);
        if(alarm_enabled)
        {
            sem_post(&Clock.start_alarm);
        }
    }
}

/* set_task: reads messages from the user interface, and 
   sets the clock, or exits the program */ 
void * set_thread(void *unused)
{
    /* message array */ 
    char message[SI_UI_MAX_MESSAGE_SIZE]; 

    /* time read from user interface */ 
    int hours, minutes, seconds; 

    /* set GUI size */ 
    si_ui_set_size(400, 200); 

    while(1)
    {
        /* read a message */ 
        si_ui_receive(message); 
        /* check if it is a set message */ 
        if (strncmp(message, "set", 3) == 0)
        {
            time_from_set_message(message, &hours, &minutes, &seconds); 
            if (time_ok(hours, minutes, seconds))
            {
                clock_set_time(hours, minutes, seconds); 
            }
            else
            {
                si_ui_show_error("Illegal value for hours, minutes or seconds"); 
            }
        }
        else if (strncmp(message, "alarm", 5) == 0)
        {
            time_from_alarm_message(message, &hours, &minutes, &seconds);
            if (time_ok(hours, minutes, seconds))
            {
                clock_set_alarm(hours, minutes, seconds); 
            }
            else
            {
                si_ui_show_error("Illegal value for hours, minutes or seconds"); 
            }
        }
        else if (strcmp(message, "reset") == 0)
        {
            clock_reset_alarm();        
        }
        /* check if it is an exit message */ 
        else if (strcmp(message, "exit") == 0)
        {
            exit(0); 
        }
        /* not a legal message */ 
        else 
        {
            si_ui_show_error("unexpected message type"); 
        }
    }
}

/* main */ 
int main(void)
{
    /* initialise UI channel */ 
    si_ui_init(); 

    /* initialise variables */         
    clock_init(); 

    /* create tasks */ 
    pthread_t clock_thread_handle;
    pthread_t alarm_thread_handle;
    pthread_t set_thread_handle;

    pthread_create(&clock_thread_handle, NULL, clock_thread, 0);
    pthread_create(&alarm_thread_handle, NULL, alarm_thread, 0);
    pthread_create(&set_thread_handle, NULL, set_thread, 0);

    pthread_join(clock_thread_handle, NULL);
    pthread_join(alarm_thread_handle, NULL);
    pthread_join(set_thread_handle, NULL);
    /* will never be here! */ 
    return 0; 
}

