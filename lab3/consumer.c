//+
//    ELEC 377,  Lab 3
//
//  consumer.c
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

    // put your code here...
	// printf("C_initTest_in\n");
	getMutex(&sharedPtr->lock);
	int numProd = sharedPtr->numProducers;
	printf("the number of prod is %d\n", numProd);
	releaseMutex(&sharedPtr->lock);
	// printf("C_initTest_out\n");
	int charRead = TRUE;
	char c;
	while (numProd > 0 && charRead)
	{
		charRead = FALSE;
		// printf("C_whileTest_1\n");
		// printf("1_charread is %d\n", charRead);
		while (charRead == FALSE && numProd > 0)
		{
			getMutex(&sharedPtr->lock);
			// printf("C_whileTest_2\n");
			if (sharedPtr->count > 0)
			{
				// printf("C_startread\n");
				c = sharedPtr->buffer[sharedPtr->out];
				sharedPtr->out = (sharedPtr->out + 1) % BUFFSIZE;
				sharedPtr->count --;
				charRead = TRUE;
				/* code */
			}else
			{
				// printf("C_endread\n");
				numProd = sharedPtr->numProducers;
				/* code */
			}
			releaseMutex(&sharedPtr->lock);
			/* code */
			putchar(c);
		}
		// printf("3_charread is %d\n", charRead);
		/* code */
	}
	// printf("C_whileTest_3\n");
	return 0;
}


