#ifndef LIFT_H
#define LIFT_H

#include <pthread.h>


#define QUEUE_UI 0
#define QUEUE_LIFT 1
#define QUEUE_FIRSTPERSON 10



/* fig_begin lift_h_defs */ 
/* size of building */ 
#define N_FLOORS 5

/* maximum number of persons in the lift system */ 
#define MAX_N_PERSONS 10  //MAX 71??

/* maximum number of passengers in lift */ 
#define MAX_N_PASSENGERS 5


/*maximum number of iterations to print out*/
#define PLANNED_TRIPS 100

#define MAX_ITERATIONS 10000/PLANNED_TRIPS


/* fig_end lift_h_defs */ 
typedef enum {LIFT_TRAVEL, // A travel message is sent to the list process when a person would
	                   // like to make a lift travel
	      LIFT_TRAVEL_DONE, // A travel done message is sent to a person process when a
	                        // lift travel is finished
	      LIFT_MOVE         // A move message is sent to the lift task when the lift shall move
	                        // to the next floor
} lift_msg_type; 

struct lift_msg{
	lift_msg_type type;  // Type of message
	int person_id;       // Specifies the person
	int from_floor[PLANNED_TRIPS];      // Specify source and destion for the LIFT_TRAVEL message.
	int to_floor[PLANNED_TRIPS];
	int trip;
};

struct ui_msg{
	int id;       // Specifies the person
	int time;
};

/* fig_begin person_data_type */ 
/* data structure for person information */ 
typedef struct
{
    /* identity */ 
    int id; 
    /* destination floor */ 
    int to_floor; 
} person_data_type; 
/* fig_end person_data_type */ 

/* special numbers, to define no identity and no destination */ 
#define NO_ID -1
#define NO_FLOOR -1

/* fig_begin lift_mon_type */ 
/* definition of monitor data type for lift */

typedef struct
{
    /* the floor where the lift is positioned */ 
    int floor; 

    /* a flag to indicate if the lift is moving */ 
    int moving; 

    /* variable to indicate if the lift is travelling in the up 
       direction, which is defined as the direction where the 
       floor number is increasing */
    int up;

    /* persons on each floor waiting to enter */ 
    person_data_type persons_to_enter[N_FLOORS][MAX_N_PERSONS];

    /* passengers in the lift */
    person_data_type passengers_in_lift[MAX_N_PASSENGERS];

    /* mutex for mutual exclusion */
    pthread_mutex_t mutex; 

    /* condition variable, to indicate that something has happend */ 
    pthread_cond_t change; 

} lift_data_type;

typedef lift_data_type* lift_type;
/* fig_end lift_mon_type */ 

/* lift_create: creates and initialises a variable of type lift_type */
lift_type lift_create(void); 

/* lift_delete: deallocates memory for lift */
void lift_delete(lift_type lift); 

/* fig_begin mon_functions */ 
/* MONITOR function lift_next_floor: computes the floor to which 
   the lift shall travel. The parameter *change_direction 
   indicates if the direction shall be changed */
void lift_next_floor(
    lift_type lift, int *next_floor, int *change_direction); 

/* MONITOR function lift_move: makes the lift move from its current 
   floor to next_floor. The parameter change_direction indicates if 
   the move includes a change of direction. This function shall be 
   called by the lift process when the lift shall move */ 
void lift_move(
    lift_type lift, int next_floor, int change_direction); 

/* MONITOR function lift_travel: makes the person with id id perform 
   a journey with the lift, starting at from_floor and ending 
   at to_floor */ 
void lift_travel(
    lift_type lift, int id, int from_floor, int to_floor);

int n_passengers_in_lift(lift_type lift);

/* enter_floor: makes a person with id id stand at floor floor */ 
void enter_floor(
    lift_type lift, int id, int floor, int to_floor);

void leave_floor(
    lift_type lift, int id, int enter_floor);

void enter_lift(lift_type lift, int id, int to_floor);
/* fig_end mon_functions */ 

// void evacuate_passengers(struct lift_msg *, lift_type);

#endif
