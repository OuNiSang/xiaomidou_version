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
#define ENTERING_READ_PROC "<> entering my_read_proc, fpos = %d\n"
#define Kbyte 1024

static struct task_struct *firstTask, *theTask;

int cnt;

//+
//arugs:
//page:     is a pointer to a kernel buffer for the module to store the data
//          to be sent to the process
//blen:     gives the length of this buffer, init is 4k
//start:    tell the kernel where the other memory is
//fpos:     tell the module what the current position is in the file,
//          init is 0
//data:     is used to point to data that tracks multiple processes reading the
//          file at once
//-
int my_read_proc(char * page, char **start, off_t fpos, int blen, int * eof, void * data){

    int numChars;
    printk(ENTERING_READ_PROC,fpos);
    if (fpos == 0){
        // // //step 1
        // numChars = sprintf(page, "Hello");
        // numChars += sprintf(page + numChars, " World\n");

        // //step2
	    // // write headers
        numChars = sprintf(page, "Step2 Test\n");
        numChars += sprintf(page + numChars,"PID\tUID\tVSZ\tRSS\n");
	    // find first task
        theTask = &init_task;
        while (theTask->pid == 0){
            theTask = theTask->next_task;
        }
        // write first task
        firstTask = theTask;

        //print all the data for the cuurent task
        numChars += sprintf(page + numChars, "%4d\t%4d\t", theTask->pid, theTask->uid);
        //check mm is NULL
        if (theTask->mm == NULL){
            numChars += sprintf(page + numChars, "%4d\t%4d\t\n",00,00);
        }else{
            numChars += sprintf(page + numChars," %4d\t%4d\t\n",
                                (theTask->mm->total_vm *4 * Kbyte)>>10,
                                (theTask->mm->rss *4 * Kbyte)>>10);
        }

        // advance to next task
        do
        {
            theTask = theTask->next_task;
        } while (theTask->pid == 0);
    } else {

        //check whether reached to the end of the list
        if (theTask == firstTask){
            *eof = 0;
            *start = page;
            return 0;
        }

        //print all the data for the current task
        numChars = sprintf(page, "%4d\t%4d\t", theTask->pid, theTask->uid);
        //check mm is NULL
        if (theTask->mm == NULL){
            numChars += sprintf(page + numChars, "%4d\t%4d\t\n",00,00);
        }else{
            numChars += sprintf(page + numChars," %4d\t%4d\t\n",
                                (theTask->mm->total_vm *4 * Kbyte)>>10,
                                (theTask->mm->rss *4 * Kbyte)>>10);
        }

	    // advance to next task
        do
        {
            theTask = theTask->next_task;
        } while (theTask->pid == 0);

    }
    *eof = 1;
    *start = page;
    return numChars;
}

int init_module(){

   struct proc_dir_entry * proc_entry;
   proc_entry = create_proc_entry("lab2",0444,NULL);//0444 == R, !W/E by everyone;NULL == dir/proc directory
   if (proc_entry == NULL){
       return -1;
       /* If there was a problem it will return NULL */
   }

   proc_entry->read_proc = my_read_proc;
   return 0;
   /*then you set the read_proc entry of the proc_entry data
     structure to your read proc function, return the value 0 */
}

void cleanup_module(){

    remove_proc_entry("lab2",NULL);

}
