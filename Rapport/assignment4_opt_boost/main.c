#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include "lift.h"
#include "si_ui.h"
#include "messages.h"
#include <sys/time.h>
#include <unistd.h>

#include "draw.h"

// These variables keeps track of the process IDs of all processes
// involved in the application so that they can be killed when the
// exit command is received.
static pid_t lift_pid;
static pid_t liftmove_pid;
static pid_t person_pid[MAX_N_PERSONS];

// Since we use processes now and not 
static int get_random_value(int person_id, int maximum_value)
{
	return rand() % (maximum_value + 1);
}


// Initialize the random seeds used by the get_random_value() function
// above.
static void init_random(void)
{
	srand(getpid()); // The pid should be a good enough initialization for
                         // this case at least.
}

static int all_done(int *done){
	int i;
	for(i = 0; i < MAX_N_PERSONS; i++){
		if(!done[i]){
			return 0;
		}
	}
	return 1;
}

static void dump_outdata(int *outdata)
{
	FILE *fp = fopen("output.txt","w");
	int i;
	fprintf(stderr, "Dumping outdata\n");
	
	for(i=0; i < MAX_N_PERSONS; i++){
		fprintf(fp, "%d ", outdata[i]);
		fprintf(fp, "\n");
	}
	fclose(fp);
}


static void liftmove_process(void)
{
	struct lift_msg m;
	m.type = LIFT_MOVE;
	while(1){
		// TODO:
		//    Sleep 2 seconds
                //    Send a message to the lift process to move the lift.
		//usleep(2000000);
		message_send((char *) &m, sizeof(m), QUEUE_LIFT, 0);
	}
}


static void lift_process(void)
{
        lift_type Lift;
	Lift = lift_create();
	int change_direction, next_floor;	
	char msgbuf[4096];
	struct lift_msg travel_list[MAX_N_PERSONS];

	while(1){
		int i;

		struct lift_msg reply;
		struct lift_msg *m;
		int len = message_receive(msgbuf, 4096, QUEUE_LIFT); // Wait for a message
		if(len < sizeof(struct lift_msg)){
			fprintf(stderr, "Message too short\n");
			continue;
		}
		
		m = (struct lift_msg *) msgbuf;
		switch(m->type){
		case LIFT_MOVE:
			// TODO: 
			//    Check if passengers want to leave elevator
                        //        Remove the passenger from the elevator
                        //        Send a LIFT_TRAVEL_DONE for each passenger that leaves
                        //        the elevator
			reply.type = LIFT_TRAVEL_DONE;
    
    			for(i = 0; i < MAX_N_PASSENGERS; i++){
        			if (Lift->passengers_in_lift[i].to_floor == Lift->floor)
        			{
					int id = Lift->passengers_in_lift[i].id;
					(travel_list[id].trip)++;
    					Lift->passengers_in_lift[i].id = NO_ID; 
    					Lift->passengers_in_lift[i].to_floor = NO_FLOOR;
					if(travel_list[id].trip == PLANNED_TRIPS){
						message_send((char *) &reply, sizeof(reply), QUEUE_FIRSTPERSON + id ,0);
					}
					else if(travel_list[id].trip > PLANNED_TRIPS){
						exit(1);
					}
					else{
						enter_floor(Lift, id, travel_list[id].from_floor[travel_list[id].trip], travel_list[id].to_floor[travel_list[id].trip]);
					}
					
        			}
    			}
			

			//    Check if passengers want to enter elevator
                        //        Remove the passenger from the floor and into the elevator
			for(i = 0; i < MAX_N_PERSONS; i++){
				person_data_type person = Lift->persons_to_enter[Lift->floor][i];
        			if (person.id != NO_ID && (n_passengers_in_lift(Lift) < MAX_N_PASSENGERS))
        			{
					leave_floor(Lift, person.id, Lift->floor);
					enter_lift(Lift, person.id, person.to_floor);
        			}
    			}
			//    Move the lift


			lift_next_floor(Lift, &next_floor, &change_direction);
			lift_move(Lift, next_floor, change_direction);

			break;
		case LIFT_TRAVEL:
                        // TODO:
                        //    Update the Lift structure so that the person with the given ID  is now present on the floor
			travel_list[m->person_id]=*m;
			enter_floor(Lift, m->person_id, m->from_floor[0], m->to_floor[0]);

			break;
		default:
			fprintf(stderr, "Error: unkown message type sent!!!!! \n");
		break;
		}
		
	}
	return;
}

static void person_process(int id)
{	
	

	init_random();
	char buf[4096];
	struct lift_msg m;

	int iterations = 0;
	struct timeval starttime;
    	struct timeval endtime;
    	long long int timediff = 0;
	long long int total_time = 0;	

	while(1){
		// TODO:
		//    Generate a to and from floor
		//    Send a LIFT_TRAVEL message to the lift process
                //    Wait for a LIFT_TRAVEL_DONE message
		//    Wait a little while
	
    		gettimeofday(&starttime, NULL);
		int i;
		for(i = 0; i < PLANNED_TRIPS; i++){
			m.from_floor[i] = get_random_value(id, N_FLOORS-1);
			m.to_floor[i] = get_random_value(id, N_FLOORS-1);
			while(m.from_floor[i] == m.to_floor[i]){
				m.to_floor[i] = get_random_value(id, N_FLOORS-1);
			}
			
		}

		m.type = LIFT_TRAVEL;
		m.person_id = id;
		m.trip=0;
		//m.from_floor = from_floor;
		//m.to_floor = to_floor;
		//fprintf(stderr,"Size of message: %d\n", sizeof(m));
		message_send((char *) &m, sizeof(m), QUEUE_LIFT, 0);
		int len = message_receive(buf, 4096, QUEUE_FIRSTPERSON+id); // Wait for a message
		if(len < sizeof(struct lift_msg)){
			fprintf(stderr, "Message too short\n");
			continue;
		}
		//fprintf(stderr, "BEFORE SLEEP\n");
		//usleep(5000000);
		//fprintf(stderr, "AFTER SLEEP\n");

		gettimeofday(&endtime, NULL);
    		timediff = (endtime.tv_sec*1000000ULL + endtime.tv_usec) -
               		(starttime.tv_sec*1000000ULL + starttime.tv_usec);
		total_time = total_time + timediff;
		if(iterations == MAX_ITERATIONS){
			struct ui_msg m_ui;
			m_ui.id = id;
			m_ui.time = total_time;
			fprintf(stderr,"ID %d done! Took %d us.\n", id, total_time);
			message_send((char *) &m_ui, sizeof(m_ui), QUEUE_UI, 0);
			
		}
		iterations++;
	}
}

// This is the final process called by main()
// It is responsible for:
//   * Receiving and executing commands from the java GUI
//   * Killing off all processes when exiting the application
void uicommand_process(void)
{

	char msgbuf[4096];

	// Data output and done
	int output[MAX_N_PERSONS];
	int done[MAX_N_PERSONS];

	//Init persons
	int i;
	int current_person_id = 0;
	for(current_person_id = 0; current_person_id < MAX_N_PERSONS; current_person_id++){
		person_pid[current_person_id] = fork();
		if(!person_pid[current_person_id]) {
			person_process(current_person_id);
		}
	}

	while(1){
		struct ui_msg *m;
		int len = message_receive(msgbuf, 4096, QUEUE_UI); // Wait for a message
		if(len < sizeof(struct ui_msg)){
			fprintf(stderr, "Message too short\n");
			continue;
		}
		
		m = (struct ui_msg *) msgbuf;
		
		output[m->id]=m->time;
		done[m->id]=1;

		if(all_done(done)){
			dump_outdata(output);
			// The code below sends the SIGINT signal to
			// all processes involved in this application
			// except for the uicommand process itself
			// (which is exited by calling exit())
			kill(lift_pid, SIGINT);
			kill(liftmove_pid, SIGINT);
			for(i=0; i < MAX_N_PERSONS; i++){
				if(person_pid[i] > 0){
					kill(person_pid[i], SIGINT);
				}
			}
			exit(0);		
		}

	}
}



int main(int argc, char **argv)
{
	message_init();

	lift_pid = fork();
	if(!lift_pid) {
		lift_process();
	}
	liftmove_pid = fork();
	if(!liftmove_pid){
		liftmove_process();
	}
	uicommand_process();

	return 0;
}
