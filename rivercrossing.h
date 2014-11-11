/***********************************************
*	IOS Project 2 - 8.4.2014
*	File: rivercrossing.h
*	Author: Lukáš Rendvanský, FIT
*	E-mail: xrendv00@stud.fit.vutbr.cz
*	Compiled with: gcc 4.7.2-1
*	Declaration of functions used in file rivercrossing.c.
************************************************/

/*
 *  Including used libraries and files.
 */

#include <stdlib.h> // For standard functions
#include <stdio.h> // For input and output
#include <fcntl.h> // For O_* constants
#include <sys/types.h> // System data types
#include <sys/stat.h> // For mode constants 
#include <stdbool.h> // For boolean values
#include <limits.h> // For max integer values
#include <string.h> // For string utilities
#include <time.h> // For seeding pseudo-random values
#include <errno.h> // For error messages

#include <sys/mman.h>

#include <unistd.h> // create child process
// pid_t fork(void);

#include <sys/wait.h> // wait, waitpid, waitid - wait for process to change state
// pid_t wait(int *status);
// pid_t waitpid(pid_t pid, int *status, int options);
// int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

#include <sys/shm.h> // shmat, shmdt - System V shared memory operations
//.....................	shmget - allocates a System V shared memory segmen
// void *shmat(int shmid, const void *shmaddr, int shmflg);
// int shmdt(const void *shmaddr); 
// int shmget(key_t key, size_t size, int shmflg);

#include <sys/sem.h> // semctl - System V semaphore control operations 
//.....................	semget - get a System V semaphore set identifier
// int semctl(int semid, int semnum, int cmd, ...);
// int semget(key_t key, int nsems, int semflg);

#include <semaphore.h> // sem_open - initialize and open a named semaphore
// sem_t *sem_open(const char *name, int oflag);
// sem_t *sem_open(const char *name, int oflag  mode_t mode, unsigned int value);


/**
 * struct type semstruct_t
 * contains all shared semaphores
 */
typedef struct{
	sem_t sem_print; // printing to file 
	sem_t sem_pier; // access to pier
	sem_t sem_board; // boarding to ship
	sem_t sem_sail; // wait for captain
	sem_t sem_landing; // landing
	sem_t sem_finish; // finished
} semstruct_t;

/**
 * struct type count_t
 * contains all shared variables
 */
typedef struct{
	pid_t hacker_main;
	pid_t serf_main;
	pid_t *pids_hackers;
	pid_t *pids_serfs;
	FILE *output; // output file
	unsigned total; // main counter
	unsigned pier_hackers; // hackers waiting on pier
	unsigned pier_serfs; // serfs waiting on pier
	unsigned boarded; // boarded persons
	unsigned landed; // landed persons
	unsigned remaining; // remaining to land
	unsigned hscount; // all persons
	unsigned hacker_time; // max time for hacker generation
	unsigned serf_time; // max time for serf generation
	unsigned sail_time; // max time for sail length
} count_t;

/**
 *	Function CheckArgs
 *
 *	void CheckArgs(int argc, char **argv, int *hs_count, 
 *		int *hacker_time, int *serif_time, int *sail_time);
 *
 *	@brief	Function checks arguments.
 *	@param	argc	Number of arguments
 *	@param	argv	Array of arguments
 *	@param	hs_count	Number of created hacker/serf processes.
 *	@param	hacker_time	Max time length (in ms) before new hacker
 *						process is generated.
 *	@param	serif_time	Max time length (in ms) before new serf
 *						process is generated.
 *	@param	sail_time	Max time length (in ms) of 'sail'.
 *	@post	Populate integer references and return or print error 
 *						message and call InputError.
 */

#ifndef RIVERCROSSING_H_CheckArgs
#define RIVERCROSSING_H_CheckArgs

	void CheckArgs(int argc, char **argv, int *hs_count, 
		int *hacker_time, int *serif_time, int *sail_time);

#endif /* RIVERCROSSING_H_CheckArgs */

/**
 *	Function HackerProcess
 *
 *	void HackerProcess(int hacker_count, semstruct_t *semaphore, count_t *counter);
 *
 *	@brief	Function simulates hacker's sail throught the river
 *	@param	hascker_count	Sequence number of hacker.
 *	@param	semaphore	Pointer to shared structure containing semaphores.
 *	@param	counter 	Pointer to shared structure containing variables.
 */

#ifndef RIVERCROSSING_H_HackerProcess
#define RIVERCROSSING_H_HackerProcess

void HackerProcess(int hacker_count, semstruct_t *semaphore, count_t *counter);

#endif /* RIVERCROSSING_H_HackerProcess */

/**
 *	Function SerfProcess
 *
 *	void SerfProcess(int serf_count, semstruct_t *semaphore, count_t *counter);
 *
 *	@brief	Function simulates hacker's sail throught the river
 *	@param	serf_count	Sequence number of serf.
 *	@param	semaphore	Pointer to shared structure containing semaphores.
 *	@param	counter 	Pointer to shared structure containing variables.
 */

#ifndef RIVERCROSSING_H_SerfProcess
#define RIVERCROSSING_H_SerfProcess

void SerfProcess(int serf_count, semstruct_t *semaphore, count_t *counter);

#endif /* RIVERCROSSING_H_SerfProcess */


/**
 *	Function semaphores_init
 *
 *	semstruct_t *semaphores_init(int *shm_id1);
 *
 *	@brief	Function allocates shared memory for structure semstruct_t and initializes semaphores.
 *	@param	shm_id1	Pointer to identification number of shared memory.
 *	@return Pointer to initialized shared memory block. 
 */

#ifndef RIVERCROSSING_H_semaphores_init
#define RIVERCROSSING_H_semaphores_init

semstruct_t *semaphores_init(int *shm_id1);

#endif /* RIVERCROSSING_H_semaphores_init */


/**
 *	Function semaphores_destroy
 *
 *	void semaphores_destroy(int *shm_id1, semstruct_t *sem_struct);
 *
 *	@brief	Function frees shared memory for structure semstruct_t and destroys semaphores.
 *	@param	shm_id1	Pointer to identification number of shared memory.
 *	@param	sem_struct	Pointer to initialized shared memory block. 
 */

#ifndef RIVERCROSSING_H_semaphores_destroy
#define RIVERCROSSING_H_semaphores_destroy

void semaphores_destroy(int *shm_id1, semstruct_t *sem_struct);

#endif /* RIVERCROSSING_H_semaphores_destroy */


/**
 *	Function counter_init
 *
 *	count_t *counter_init(int *shm_id2, unsigned hs_count, unsigned hacker_time, 
 *			unsigned serf_time, unsigned sail_time);
 *
 *	@brief	Function allocates shared memory for structure count_t and initializes semaphores.
 *	@param	shm_id2	Pointer to identification number of shared memory.
 *	@param	hs_count	Number of hackers and serfs generated.
 *	@param	hacker_time	Maximal time of hacker generation
 *	@param	serf_time	Maximal time of serf generation
 *	@param	sail_time	Maximal time of sail
 *	@return Pointer to initialized shared memory block. 
 */

#ifndef RIVERCROSSING_H_counter_init
#define RIVERCROSSING_H_counter_init

count_t *counter_init(pid_t *pids_hackers, pid_t *pids_serfs, int *shm_id2,
	unsigned hs_count, unsigned hacker_time, unsigned serf_time, unsigned sail_time);

#endif /* RIVERCROSSING_H_counter_init */


/**
 *	Function counter_destroy
 *
 *	void counter_destroy(int *shm_id2, count_t *counter);
 *
 *	@brief	Function frees shared memory for structure count_t.
 *	@param	shm_id1	Pointer to identification number of shared memory.
 *	@param	counter	Pointer to initialized shared memory block. 
 */

#ifndef RIVERCROSSING_H_counter_destroy
#define RIVERCROSSING_H_counter_destroy

void counter_destroy(int *shm_id2, count_t *counter);

#endif /* RIVERCROSSING_H_counter_destroy */

void Kill_Them_All(count_t *counter);

/* End of file rivercrossing.h */
