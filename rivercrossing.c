/***********************************************
*	IOS Project 2 - 8.4.2014
*	File: rivercrossing.c
*	Author: Lukáš Rendvanský, FIT
*	E-mail: xrendv00@stud.fit.vutbr.cz
*	Compiled with: gcc 4.7.2-1
*	Main file.
************************************************/

/*
 *  Including used headers.
 */

#include "rivercrossing.h"
#include "error.h"
#include "strtonum.h"

/*
 * Defining constant messages
 */

const char *filename = "rivercrossing.out";
const char *wrong_num="Incorrect number of arguments.";
const char *wrong_arg="Invalid arguments.";
const char *err_msg="Run with argument --help for help.";
const char *help_msg="\nIOS Project 2 - River crossing problem.\n"
	"Created: 8.4.2014\nAuthor: Lukas Rendvansky, FIT VUT Brno\n"
	"E-mail: xrendv00@stud.fit.vutbr.cz"
	"\n\nDescription:\n"
	"Modified synchronization problem called \"River crossing problem\" "
	"implemented in C language. Program creates two child processes "
	"(categories hackers and serfs), which will be creating their own "
	"child processes (count given by argument P), after given period of time "
	"(maximum value in ms is given by arguments H an S). These hackers "
	"and serfs are being boarded on ship by groups of four persons, "
	"each consisting of at least 2 persons of any category to board (e.g. "
	"OH 4S, 2H 2S, 4H 0S). The last person that creates group become captain and "
	"becomes freezed for maximum of R miliseconds (time of sail). After this time, "
	"all processes exit the ship and become ignored. New precosses are being boarded "
	"by same rule. The program closes with exit code 0 if all procesess "
	"successfuly end, or it is terminated with exit code 2 if any of system calls fails."
	"\n\nUsage: ./rivercrossing --help or ./rivercrossing P H S R"
	"\n--help\tprints this help message."
	"\nP\tnumber of persons created in each category ((P > 0) && (P % 2 == 0))."
	"\nH\tmax time (in ms) before new hacker process is generated."
	"\nS\tmax time (in ms) before new serf process is generated."
	"\nR\tmax time (in ms) of sail.\n\n";

/*
 * Program start - function main() 
 */

int main(int argc, char **argv)
{
	pid_t hacker_main_pid = 0, serf_main_pid = 0, hacker_gen_pid, serf_gen_pid; // PIDs for hackers and serfes generators
	semstruct_t *sem_struct = NULL; // semaphore
	count_t *counter = NULL; 
    int shm_id1, shm_id2;
	int hs_count, hacker_time, serf_time, sail_time; //user input
	int hacker_count = 0, serf_count = 0; // Counter of created hackers and serfs

	// check if all arguments are correct
	CheckArgs(argc,argv,&hs_count,&hacker_time,&serf_time,&sail_time);

	pid_t pids_hackers[hs_count];
	pid_t pids_serfs[hs_count];

	for(int i=0; i<(hs_count/2); i++)
		pids_hackers[i]=0;

	for(int i=0; i<(hs_count/2); i++)
		pids_serfs[i]=0;

	sem_struct = semaphores_init(&shm_id1);
	counter = counter_init(pids_hackers, pids_serfs, &shm_id2,(unsigned) hs_count, 
		(unsigned) hacker_time, (unsigned) serf_time, (unsigned) sail_time);

	// seed pseudo-random values
	srand (time(NULL));
	
	hacker_main_pid = fork();
    switch(hacker_main_pid)
    {

    	case -1:
    		SyscallError(stderr,"Unable to create child process.");
    		break;

    	case 0:
	    {	
	    	for(hacker_count=0; hacker_count < hs_count; hacker_count++)
	    	{
	    		usleep((rand() % (int)(hacker_time + 1)));
			    hacker_gen_pid = fork();
			    switch(hacker_gen_pid)
			    {
			    	case -1:
			    	{
			    		Kill_Them_All(counter);
			    		SyscallError(stderr,"Unable to create child process.");
			    	} break;
			    	
			    	case 0:
			    		HackerProcess(hacker_count, sem_struct, counter);
						break;   	
					default:
						counter->pids_hackers[hacker_count] = hacker_gen_pid;
						break;
			    }
	    	}

			fclose(counter->output);

        	for(int i=0; i<hs_count;i++)
				wait(NULL);

	    	exit(0);
	    } break;

	    default:
	    {
	        serf_main_pid = fork();
	        switch(serf_main_pid)
	       	{	
		    	case -1:
		    	{
		    		Kill_Them_All(counter);
		    		SyscallError(stderr,"Unable to create child process.");
		    	}

		        case 0:
		        {
		    		for(serf_count=0; serf_count < hs_count; serf_count++)
		    		{
		    			usleep((rand() % (int)(serf_time + 1)));
					    serf_gen_pid = fork();
					    switch(serf_gen_pid)
					    {
					    	case -1:
					    	{
					    		Kill_Them_All(counter);
					    		SyscallError(stderr,"Unable to create child process.");
					    	} break;

					    	case 0:
					    		SerfProcess(serf_count, sem_struct, counter);
								break;
							default:
								counter->pids_serfs[serf_count] = serf_gen_pid;
								break;
					    }

			    	}

		        	for(int i=0; i<hs_count;i++)
						wait(NULL);
	
					fclose(counter->output);

			    	exit(0);
		        } break;

		        default:
	            {	
	            	counter->hacker_main = hacker_main_pid;
	            	counter->serf_main = serf_main_pid;
	            	wait(NULL);
	            	wait(NULL);
	            }
	       	}
	    } break;
	}		

	semaphores_destroy(&shm_id1,sem_struct);
	counter_destroy(&shm_id2,counter);

	return 0;
}

/* 
 * Functions (description in header file) 
 */

void CheckArgs(int argc, char **argv, int *hs_count, int *hacker_time, 
				int *serf_time, int *sail_time)
{
	long long __hs_count, __hacker_time, __serf_time, __sail_time;
	bool invalid = false;
	const char *errstr[4] = {NULL};

	// test for help and print help message
	if((argc == 2) && (strcmp(argv[1],"--help") == 0))
		ExitMessage(stdout,"%s",help_msg);

	// test number of arguments
	if(argc != 5)
		InputError(stderr,"%s (%d). %s", wrong_num, (argc - 1), err_msg);

	// try to covert all string arguments to numbers
	__hs_count = strtonum(argv[1],2L,UINT_MAX,&errstr[0]);
	__hacker_time = strtonum(argv[2],0L,5000L,&errstr[1]);
	__serf_time = strtonum(argv[3],0L,5000L,&errstr[2]);
	__sail_time = strtonum(argv[4],0L,5000L,&errstr[3]);

	// test if converting was successful
	for(int i = 0; i < 4; i++)
	{
		// print warning with each error and set invalid to true
		if(errstr[i] != NULL)
		{ 
			Warning(stderr,"Argument %d - value %s",(i+1),errstr[i]);
			invalid = true;
			continue;
		}

		if((i == 0) && (__hs_count % 2 != 0))
		{ 
			Warning(stderr,"Argument %d - value not divisible by 2",(i + 1));
			invalid = true;
			continue;
		}
	}

	// close program with exit code 1 (invalid arguments)
	if(invalid)
		InputError(stderr,"%s %s", wrong_arg, err_msg);

	// pass successfuly converted values to main function by references
	*hs_count = (int) __hs_count;
	*hacker_time = (int) __hacker_time;
	*serf_time = (int) __serf_time;
	*sail_time = (int) __sail_time;

	return;
}

void HackerProcess(int hacker_count, semstruct_t *semaphore, count_t *counter)
{
	bool captain = false; // create local variable for captain status in each process

	sem_wait(&semaphore->sem_print);
		counter->total++;
		fprintf(counter->output,"%d: hacker: %d: started\n",counter->total,hacker_count+1);
		fflush(counter->output);
	sem_post(&semaphore->sem_print);

	sem_wait(&semaphore->sem_pier);
		sem_wait(&semaphore->sem_print);
			counter->total++;
			counter->pier_hackers++;
			fprintf(counter->output,"%d: hacker: %d: waiting for boarding: %d: %d\n",counter->total,hacker_count+1,
						counter->pier_hackers, counter->pier_serfs);
			fflush(counter->output);
		sem_post(&semaphore->sem_print);	

	if(counter->pier_hackers == 4)
	{
		captain = true;
		counter->pier_hackers = 0;

		for(int i=0; i < 3; i++)
			sem_post(&semaphore->sem_board);

		sem_wait(&semaphore->sem_print);
			counter->total++;
			fprintf(counter->output,"%d: hacker: %d: boarding: %d: %d\n",counter->total, hacker_count+1,
					counter->pier_hackers, counter->pier_serfs);
			fflush(counter->output);
			counter->boarded++;	
		sem_post(&semaphore->sem_print);
	}
	else if(counter->pier_hackers == 2 && counter->pier_serfs >=2)
	{
		captain = true;
		counter->pier_hackers = 0;
		counter->pier_serfs -= 2;

		for(int i=0; i < 3; i++)
			sem_post(&semaphore->sem_board);

		sem_wait(&semaphore->sem_print);
			counter->total++;
			fprintf(counter->output,"%d: hacker: %d: boarding: %d: %d\n",counter->total, hacker_count+1,
					counter->pier_hackers, counter->pier_serfs);
			fflush(counter->output);
			counter->boarded++;	
		sem_post(&semaphore->sem_print);
	}
	else
	{
		sem_post(&semaphore->sem_pier);
		sem_wait(&semaphore->sem_board);
		sem_wait(&semaphore->sem_print);
			counter->total++;
			fprintf(counter->output,"%d: hacker: %d: boarding: %d: %d\n",counter->total, hacker_count+1,
					counter->pier_hackers, counter->pier_serfs);
			fflush(counter->output);
			counter->boarded++;
		sem_post(&semaphore->sem_print);
	}

	if(counter->boarded == 4)
	{
		counter->boarded = 0;
		for(int i=0; i<3; i++)
			sem_post(&semaphore->sem_sail);	
	}
	else{
		sem_wait(&semaphore->sem_sail);
	}

	sem_wait(&semaphore->sem_print);
		counter->total++;
		if(captain)
			fprintf(counter->output,"%d: hacker: %d: captain\n",counter->total, hacker_count+1);
		else
			fprintf(counter->output,"%d: hacker: %d: member\n",counter->total, hacker_count+1);
		fflush(counter->output);
	sem_post(&semaphore->sem_print);

	if(captain)
	{
		usleep((rand() % (int)(counter->sail_time + 1)));
		for(int i=0; i<3; i++)
			sem_post(&semaphore->sem_landing);
	}
	else
		sem_wait(&semaphore->sem_landing);

	sem_wait(&semaphore->sem_print);
		counter->total++;
		fprintf(counter->output,"%d: hacker: %d: landing: %d: %d\n",counter->total, hacker_count+1,
				counter->pier_hackers, counter->pier_serfs);
		fflush(counter->output);
		counter->landed++;
	sem_post(&semaphore->sem_print);

	if(counter->landed == 4)
	{
		counter->landed = 0;
		sem_post(&semaphore->sem_pier);
	}

	counter->remaining--;
	if(counter->remaining > 0)
	{
		sem_wait(&semaphore->sem_finish);
	}
	else
	{
		for(unsigned i = 1; i < counter->hscount; i++)
			sem_post(&semaphore->sem_finish);
	}

	sem_wait(&semaphore->sem_print);
		counter->total++;
		fprintf(counter->output,"%d: hacker: %d: finished\n",counter->total,hacker_count+1);
		fflush(counter->output);
	sem_post(&semaphore->sem_print);

	fclose(counter->output);

	exit(0);
}

void SerfProcess(int serf_count, semstruct_t *semaphore, count_t *counter)
{
	bool captain = false;  // create local variable for captain status in each process

	sem_wait(&semaphore->sem_print);
		counter->total++;
		fprintf(counter->output,"%d: serf: %d: started\n",counter->total,serf_count+1);
		fflush(counter->output);
	sem_post(&semaphore->sem_print);

	sem_wait(&semaphore->sem_pier);
		sem_wait(&semaphore->sem_print);
			counter->total++;
			counter->pier_serfs++;
			fprintf(counter->output,"%d: serf: %d: waiting for boarding: %d: %d\n",counter->total,serf_count+1,
					counter->pier_hackers, counter->pier_serfs);
			fflush(counter->output);
		sem_post(&semaphore->sem_print);		

	if(counter->pier_serfs == 4)
	{
		captain = true;
		counter->pier_serfs = 0;

		for(int i=0; i < 3; i++)
			sem_post(&semaphore->sem_board);

		sem_wait(&semaphore->sem_print);
			counter->total++;
			fprintf(counter->output,"%d: serf: %d: boarding: %d: %d\n",counter->total, serf_count+1,
					counter->pier_hackers, counter->pier_serfs);
			fflush(counter->output);
			counter->boarded++;
		sem_post(&semaphore->sem_print);
	}
	else if(counter->pier_serfs == 2 && counter->pier_hackers >=2)
	{
		captain = true;
		counter->pier_serfs = 0;
		counter->pier_hackers -= 2;


		for(int i=0; i < 3; i++)
			sem_post(&semaphore->sem_board);

		sem_wait(&semaphore->sem_print);
			counter->total++;
			fprintf(counter->output,"%d: serf: %d: boarding: %d: %d\n",counter->total, serf_count+1,
					counter->pier_hackers, counter->pier_serfs);
			fflush(counter->output);
			counter->boarded++;
		sem_post(&semaphore->sem_print);
		
	}
	else
	{
		sem_post(&semaphore->sem_pier);
		sem_wait(&semaphore->sem_board);
		sem_wait(&semaphore->sem_print);
			counter->total++;
			fprintf(counter->output,"%d: serf: %d: boarding: %d: %d\n",counter->total, serf_count+1,
					counter->pier_hackers, counter->pier_serfs);
			fflush(counter->output);
			counter->boarded++;
		sem_post(&semaphore->sem_print);
	}

	if(counter->boarded == 4)
	{
		counter->boarded = 0;
		for(int i=0; i<3; i++)
			sem_post(&semaphore->sem_sail);	
	}
	else{
		sem_wait(&semaphore->sem_sail);
	}

	sem_wait(&semaphore->sem_print);
		counter->total++;
		if(captain)
			fprintf(counter->output,"%d: serf: %d: captain\n",counter->total, serf_count+1);
		else
			fprintf(counter->output,"%d: serf: %d: member\n",counter->total, serf_count+1);
		fflush(counter->output);
	sem_post(&semaphore->sem_print);

	if(captain)
	{
		usleep(0 + (rand() % (int)(counter->sail_time - 0 + 1)));
		for(int i=0; i<3; i++)
			sem_post(&semaphore->sem_landing);
	}
	else
		sem_wait(&semaphore->sem_landing);

	sem_wait(&semaphore->sem_print);
		counter->total++;
		fprintf(counter->output,"%d: serf: %d: landing: %d: %d\n",counter->total, serf_count+1,
				counter->pier_hackers, counter->pier_serfs);
		fflush(counter->output);
		counter->landed++;
	sem_post(&semaphore->sem_print);

	if(counter->landed == 4)
	{
		counter->landed = 0;
		sem_post(&semaphore->sem_pier);
	}

	counter->remaining--;
	if(counter->remaining > 0)
	{

		sem_wait(&semaphore->sem_finish);
	}
	else
	{

		for(unsigned i = 0; i < counter->hscount; i++)
			sem_post(&semaphore->sem_finish);
	}

	sem_wait(&semaphore->sem_print);
		counter->total++;
		fprintf(counter->output,"%d: serf: %d: finished\n",counter->total,serf_count+1);
		fflush(counter->output);
	sem_post(&semaphore->sem_print);

	fclose(counter->output);

	exit(0);
}

semstruct_t *semaphores_init(int *shm_id1)
{
	*shm_id1 = shmget(IPC_PRIVATE, sizeof(semstruct_t), IPC_CREAT | IPC_EXCL | 0666);
	if(*shm_id1 == -1)
		SyscallError(stderr,"Unable to create shared memory. Details: %s",strerror(errno));

	semstruct_t *semaphore = shmat(*shm_id1, NULL, SHM_RND);
	if(semaphore == (void *) -1)
		SyscallError(stderr,"Unable to create shared memory. Details: %s",strerror(errno));

    if (sem_init(&semaphore->sem_print, 1, 1) == -1)
        SyscallError(stderr,"Unable to initialize semaphore. Details: %s",strerror(errno));
    if (sem_init(&semaphore->sem_pier, 1, 1) == -1)
        SyscallError(stderr,"Unable to initialize semaphore. Details: %s",strerror(errno));
    if (sem_init(&semaphore->sem_board, 1, 0) == -1)
        SyscallError(stderr,"Unable to initialize semaphore. Details: %s",strerror(errno));
    if (sem_init(&semaphore->sem_sail, 1, 0) == -1)
        SyscallError(stderr,"Unable to initialize semaphore. Details: %s",strerror(errno));
    if (sem_init(&semaphore->sem_landing, 1, 0) == -1)
        SyscallError(stderr,"Unable to initialize semaphore. Details: %s",strerror(errno));
    if (sem_init(&semaphore->sem_finish, 1, 0) == -1)
        SyscallError(stderr,"Unable to initialize semaphore. Details: %s",strerror(errno));

	return semaphore;
}

void semaphores_destroy(int *shm_id1, semstruct_t *sem_struct)
{
    if (sem_destroy(&sem_struct->sem_print)==-1)
        SyscallError(stderr,"Unable to destroy semaphore. Details: %s",strerror(errno));
    if (sem_destroy(&sem_struct->sem_pier)==-1)
        SyscallError(stderr,"Unable to destroy semaphore. Details: %s",strerror(errno));
    if (sem_destroy(&sem_struct->sem_board)==-1)
        SyscallError(stderr,"Unable to destroy semaphore. Details: %s",strerror(errno));
    if (sem_destroy(&sem_struct->sem_sail)==-1)
        SyscallError(stderr,"Unable to destroy semaphore. Details: %s",strerror(errno));
    if (sem_destroy(&sem_struct->sem_landing)==-1)
        SyscallError(stderr,"Unable to destroy semaphore. Details: %s",strerror(errno));
    if (sem_destroy(&sem_struct->sem_finish)==-1)
        SyscallError(stderr,"Unable to destroy semaphore. Details: %s",strerror(errno));

    /// Detach the shared memory segment.
    if (shmdt(sem_struct) == -1)
        SyscallError(stderr,"Unable to deallocate shared memory. Details: %s",strerror(errno));

    /// Deallocate (mark to destroy) the shared memory segment.
    if (shmctl(*shm_id1, IPC_RMID, NULL) == -1)
        SyscallError(stderr,"Unable to deallocate shared memory. %d Details: %s",errno,strerror(errno));

	return;
}

count_t *counter_init(pid_t *pids_hackers, pid_t *pids_serfs, int *shm_id2, unsigned hs_count,
						unsigned hacker_time, unsigned serf_time, unsigned sail_time)
{
	

	*shm_id2 = shmget(IPC_PRIVATE, sizeof(count_t), IPC_CREAT | IPC_EXCL | 0666);
	if(*shm_id2 == -1)
		SyscallError(stderr,"Unable to create shared memory. Details: %s",strerror(errno));

	count_t *counter = shmat(*shm_id2, NULL, SHM_RND);
	if(counter == (void *) -1)
		SyscallError(stderr,"Unable to create shared memory. Details: %s",strerror(errno));
	
	counter->pids_hackers = pids_hackers;
	counter->pids_serfs = pids_serfs;
	counter->output = fopen(filename,"wb");
	counter->total = 0;
	counter->pier_hackers = 0;
	counter->pier_serfs = 0;
	counter->boarded = 0;
	counter->landed = 0;
	counter->remaining = 2*hs_count;
	counter->hscount = 2*hs_count;
	counter->hacker_time = hacker_time;
	counter->serf_time = serf_time;
	counter->sail_time = sail_time;

	return counter;
}

void counter_destroy(int *shm_id2, count_t *counter)
{
	fclose(counter->output);

	if (shmdt(counter)==-1)
        SyscallError(stderr,"Unable to deallocate shared memory. Details: %s",strerror(errno));

    if (shmctl(*shm_id2, IPC_RMID, NULL)==-1)
        SyscallError(stderr,"Unable to deallocate shared memory. Details: %s",strerror(errno));

	return;
}

void Kill_Them_All(count_t *counter)
{
	wait(NULL); 

	Warning(stderr,"Something went wrong. I must kill them all. :(");
	for(unsigned i=0; i < (counter->hscount/2); i++)
		if(counter->pids_serfs[i] != 0)
			kill(counter->pids_serfs[i], SIGTERM);

	for(unsigned i=0; i < (counter->hscount/2); i++)
		if(counter->pids_hackers[i] != 0)
			kill(counter->pids_hackers[i], SIGTERM);

	kill(counter->hacker_main, SIGTERM);
	kill(counter->serf_main, SIGTERM);

}

/* End of file rivercrossing.c */
