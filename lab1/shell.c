//+
// File:	shell.c
//
// Purpose:	This program implements a simple shell program. It does not start
//		processes at this point in time. However, it will change directory
//		and list the contents of the current directory.
//
//		The commands are:
//		   cd name -> change to directory name, print an error if the directory doesn't exist.
//		              If there is no parameter, then change to the home directory.
//		   ls -> list the entries in the current directory.
//			      If no arguments, then ignores entries starting with .
//			      If -a then all entries
//		   pwd -> print the current directory.
//		   exit -> exit the shell (default exit value 0)
//
//		if the command is not recognized an error is printed.
//-

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>

#define CMD_BUFFSIZE 1024
#define MAXARGS 10

int splitCommandLine(char * commandBuffer, char* args[], int maxargs);
void doCommand(char * args[], int nargs);
void exitFunc(char * args[], int nargs);
void pwdFunc(char *arg[], int nargs);
void cdFunc(char * args[], int nargs);
void lsFunc(char * args[], int nargs);
int checkDotDocument(struct dirent *passInFile);


int main() {

    char commandBuffer[CMD_BUFFSIZE];
    char *args[MAXARGS];

    // print prompt.. fflush is needed because
    // stdout is line buffered, and won't
    // write to terminal until newline
    printf("%%> ");
    fflush(stdout);

    while(fgets(commandBuffer,CMD_BUFFSIZE,stdin) != NULL){

		// Remove newline at end of buffer
		// TODO Step 2: remove newline from end of buffer
		char *tmp = commandBuffer;
		*(tmp+strlen(commandBuffer)-1) = '\0';

		// Split command line into words.
		// TODO Step 2: call splitCommandLine with the right parameters
		int nargs = splitCommandLine(commandBuffer,args,MAXARGS);

		// Debugging for step 2
		printf("%d\n", nargs);
		int i;
		for (i = 0; i < nargs; i++){
	 		printf("%d: %s\n",i,args[i]);

			// Execute the command
			// TODO: Step 3 call doCommand with the right arguments
			// Remember to check if there is a command (i.e. value of nargs)
			doCommand(args,nargs);

			// print prompt
			printf("%%> ");
			fflush(stdout);
    	}

		if (nargs == 0)
		{
			printf("no input is found\n");
			printf("%%> ");
			fflush(stdout);
			/* if there is no input, which nargs == 0, print this worning */
		}
		
	}
}

////////////////////////////// String Handling ///////////////////////////////////

//+
// Function:	skipChar
//
// Purpose:	This function skips over a given char in a string
//		For security, the function will not skip null chars.
//
// Parameters:
//    charPtr	Pointer to string
//    skip	character to skip
//
// Returns:	Pointer to first character after skipped chars.
//		Identity function if the string doesn't start with skip,
//		or skip is the null character
//-

char *skipChar(char * charPtr, char skip){

	if (skip == '\0'){
		return charPtr;
		/* return itself when is a NULL*/
	}else if (skip == 0x20){
		while (*charPtr == 0x20 || *(charPtr+1) == 0x20){
			charPtr++;
		}
		return charPtr;
		/* skip all the ' ' and return the first char pointer */

	}else{
		while (*charPtr != 0x20 && *charPtr != '\0'){
			charPtr++;
		}
		return charPtr;
		/* skip to find the first char and return the fisrt ' ' pointer */
	}
}
//  // TODO Step 2: skip over instances of the char skip
//  //    return input value of charPtr if *char is null char

//+
// Function:	splitCommandLine
//
// Purpose:	This splits a string into an array of strings.
//		The array is passed as an argument. The string
//		is modified by replacing some of the space characters
//		with null characters to terminate each of the strings.
//
// Parameters:
//	commandBuffer	The string to split
//	args		An array of char pointers
//	maxargs		Size of array args (max number of arguments)
//
// Returns:	Number of arguments (< maxargs).
//
//-

int splitCommandLine(char * commandBuffer, char* args[], int maxargs){

	int counter = 0;

	while (*commandBuffer != '\0')
	{
		if(*commandBuffer == 0x20)
		{
			commandBuffer =	skipChar(commandBuffer, 0x20);
			/* skip all the space to find first char*/
		}else{
			args[counter] = commandBuffer;
			commandBuffer =	skipChar(commandBuffer, *commandBuffer);
			*commandBuffer = '\0';
			commandBuffer++;
			counter++;
			/* put the first char address in the args, change the last space of char into NULL and point to the next one  */
		}

	}
	if (counter > maxargs)
	{
		printf("args overflow");
		/*check whether the args is overflowed*/
	}

	return counter;

   // TODO Step 2 split the command into words using only
   // the functions skipChar and strchr. You may use fprintf to report
   // errors that are encountered to stderr.
}


////////////////////////////// Command Handling ///////////////////////////////////

// typedef for pointer to command handling functions

// TODO STEP 3a write the typedef for the function handler pointer to
// use in the structure immediately below.
// See the description of the function prototypes at the bottom of
// the file in the comments.
typedef void (*cmdFuncPtr)(char * args[], int nargs);


// cmdType type:
// Associates a command name with a command handling function
// TODO STEP 3b use the typedef above (Step 3a) to make a two element
// struct. The first is a char * for the name of the command
// the other is a function pointer for the command handling function
struct cmdType
{
	char *cmdName;
	cmdFuncPtr cmdFunction;
	char **args;
	int nargs;
};

// prototypes for command handling functions
// TODO STEP 4b,6: add a function prototype
// for each command handling function
void callFunction(cmdFuncPtr func, char * args[], int nargs){
	func(args,nargs);
}

// Array that provides the list commands and functions
// must be terminated by {NULL, NULL}
// in a real shell, this would be a hashtable.
// TODO Step 4a: add a global array of
// the type above that contains initializers
// of strings and command handling funciton names
struct cmdType commandArrayName[] = {

	{"ls", lsFunc},
	{"cd", cdFunc},
	{"pwd", pwdFunc},
	{"exit", exitFunc},
	{NULL,NULL}
};

//+
// Function:	doCommand
//
// Purpose:	This command calls a command handling funciton from
//		the commands array based on the first argument passed
//		in the args array.
//
// Parameters:
//	args	command and parameters, an array of pointers to strings
//	nargs	number of entries in the args array
//
// Returns	nothing (void)
//-

void doCommand(char * args[], int nargs){

	int counterC = 0;
	int counterA = 0;
	// FILE *outPutFile = fopen("test1.txt","w");

	while (1)
	{
		if (commandArrayName[counterC].cmdName == NULL){
			break;
			/*hit NULL in commandArrayName*/
	 	}
		if (counterA == nargs)
		{

			break;
			/*hit the end of array, print error*/
		}
		if (strcmp(commandArrayName[counterC].cmdName,args[counterA]) == 0){
			printf("performing %s\n",commandArrayName[counterC].cmdName);
			callFunction(commandArrayName[counterC].cmdFunction,args,nargs);
			counterC = 0;
			counterA++;
			/* exicute the function and move on */
		}else{
			counterC++;
			/* move to the next command */
		}
	}
	// fprintf(outPutFile,"%s\n","Test");
	// fclose(outPutFile);
	// /*generating test files*/

   // TODO Step 5 this function is small
   //  this is the command search loop
}

//////////////////////////////////////////////////
//            command Handling Functions        //
//////////////////////////////////////////////////
// all command handling functions have the same
// parameter types and return values.
//////////////////////////////////////////////////

//+
// Function:	all handling functions
//
// Purpose:	this command performs the funcdtion
//		associated with the commands.
//		The name of the comomand is in first argument.
//	 	Since it is called by doCommand, it is known
//		that arg is at least one element long
//
// Parameters:
//	args	command and parameters, an array of pointers to strings
//	nargs	number of entries in the args array
//
// Returns	nothing (void)
//-

// TODO step 4b put command handling function for exit here
void exitFunc(char * args[], int nargs){
	exit(0);
}


// TODO step 6 put rest of command handling functions here
void pwdFunc(char *arg[], int nargs){
	char *cwd = getcwd(NULL, 0);
	printf("%s\n",cwd);
	free(cwd);
}

void cdFunc(char * args[], int nargs){

	// printf("%d",nargs);
	struct passwd *pw = getpwuid(getuid());
	if (pw->pw_dir == NULL)
	{
		printf("Error: home directory is NULL\n");
		/*report an error when home directory is NULL*/
	}else
	{
		if (chdir(pw->pw_dir) == 0)
		{
			printf("%s\n", pw->pw_dir);
			printf("Dir change success\n");
			/* code */
		}else
		{
			printf("Directory check failed\n");
			/*chdir() returns none 0 values when it can not change*/
		}
	}
}

void lsFunc(char * args[], int nargs){
	
	printf("the numbers of nargs in ls function: %d\n",nargs);
	struct dirent **namelist;
	int numEnts = scandir(".", &namelist, NULL, NULL);
	// printf("numbers %s\n",*(args+1));

	if (nargs == 1)
	{
		int count = 0;
		while (count < numEnts)
		{
		printf("there are %d in the list\n checking the number %d files\n",numEnts,count+1);
		numEnts = scandir(".", &namelist, checkDotDocument(*(namelist+count)), NULL);
		count++;
			/* code */
		}
		/* code */
	}else if (*(args+1) == "-a")
	{
		printf("there are %d in the list\n",numEnts);
		numEnts = scandir(".", &namelist, NULL, NULL);
		/* code */
	}

	// printf("there are %d in the list\n",numEnts);
	int i = 0;
	char *temp;
	for (i = 0; i < numEnts; i++)
	{
		temp = namelist[i]->d_name;
		printf("%s\n", temp);
		/* code */
	}
}

int checkDotDocument(struct dirent *passInFile){

	char *dot = ".";
	int temp = 0;
	// printf("checking file %s\n", passInFile->d_name);
	if (strncmp(passInFile->d_name,dot,1) != 0)
	{
		temp = 1;
		printf("dot file sucessfuly checked\n");
		/* code */
	}else
	{
		printf("not a dot files\n");
	}
	
	
	return (temp);
}
