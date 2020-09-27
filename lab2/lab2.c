/*+
 * Module:  lab2.c
 *
 * Purpose: Skeleton solution to ELEC 377 Lab2.
 *
-*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

static struct task_struct * firstTask, *theTask;

int cnt;

int my_read_proc(char * page, char **start, off_t fpos, int blen, int * eof, void * data){

    int numChars;
    if (fpos == 0){
	     // write headers
	    // find first task
        // write first task
        // advance to next task
    } else {
        //if (at back at begining of list){
            *eof = 0;
            *start = page;
            return 0;
        //}
	    // write task info for one task
	    // advance to next task
    }
    *eof = 1;
    *start = page;
    return numChars;
}

int init_module(){
   struct proc_dir_entry * proc_entry;
}

void cleanup_module(){
}
