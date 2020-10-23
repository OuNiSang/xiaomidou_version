//+
//	ELEC 377,  Lab 3
//
//  producer.c
//-

#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <errno.h>

#include "common.h"

#define FALSE 0
#define TRUE 1


int main (int argc, char *argv[]){

	// initialize the shared memory, load in the initial array's, spawn the worker
	// processes.

	key_t   key;
	struct shared    *sharedPtr;
	int shmid;
	int c,stored;
    
	/*	 Shared memory init 	*/
	key = ftok(".", 'S');
	if((shmid = shmget(key, MEMSIZE, IPC_CREAT|0666)) == -1 ){
		if( (shmid = shmget(key, MEMSIZE, 0)) == -1){
			printf("Error allocating shared memory. \n");
			exit(1);
		}
	}

	// now map the region..
	if((int)(sharedPtr = (struct shared *) shmat(shmid, 0, 0)) == -1){
	    printf("Couldn't map the memory into our process space.\n");
	    exit(1);
	}
	
    // put your code here..
	getMutex(&sharedPtr->lock);
	sharedPtr->numProducers++;
	releaseMutex(&sharedPtr->lock);
	printf("P_initTest_1");
	while ((c = getchar()) != EOF){
		stored = FALSE;
		printf("P_whileTest_1");
		while (stored == FALSE){
			getMutex(&sharedPtr->lock);
			printf("P_whileTest_2");
			if (sharedPtr->count < BUFFSIZE){	
				sharedPtr->buffer[sharedPtr->in] = c;
				sharedPtr->in++;
				sharedPtr->count++;
				stored = TRUE;
				/* code */
			}
			releaseMutex(&sharedPtr->lock);
			/* code */
		}
		/* code */
	}
	printf("P_whileTest_3");
	getMutex(&sharedPtr->lock);
	sharedPtr->numProducers--;
	releaseMutex(&sharedPtr->lock);
	return 0;
}


