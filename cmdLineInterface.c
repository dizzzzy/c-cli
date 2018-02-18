#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

//gcc -o -g cmdLineInterface cmdLineInterface.c

#define MAX_LINE		80 // 80 chars per line
#define MAX_COMMANDS	7 // size of history 

char history[MAX_COMMANDS][MAX_LINE];
char display_history [MAX_COMMANDS][MAX_LINE];

int command_count = 0;

void addtohistory(char inputBuffer[]) {
	int i = 0;
	strcpy(history[command_count % MAX_COMMANDS], inputBuffer); //make sure u dont pass max command 
	while (inputBuffer[i] != '\n' && inputBuffer[i] != '\0') { //not end or enter
		display_history[command_count % MAX_COMMANDS][i] = inputBuffer[i]; 
		i++;
	}
	display_history[command_count % MAX_COMMANDS][i] = '\0';
	
	++command_count;

	return;
}


void setup(char inputBuffer[], char *args[],int *background)
{
    int length, i, start, char_index, command_number;
    
    char_index = 0;
	
	do {
		printf("osh>");
		fflush(stdout);
		length = read(STDIN_FILENO,inputBuffer,MAX_LINE); //calculating length of command
	}
	while (inputBuffer[0] == '\n'); /* swallow newline characters */
	
	
    if (length == 0 || length < 0){
        exit(0);         
	}
	
	if (inputBuffer[0] == '!') { //check for history using !
		if (command_count == 0) {
			printf("No history\n");
		}
		else if (inputBuffer[1] == '!') {
			// restore the previous command
			strcpy(inputBuffer,history[(command_count - 1) % MAX_COMMANDS]);
			length = strlen(inputBuffer) + 1;
		}
		else if (isdigit(inputBuffer[1])) { /* retrieve the nth command */
			command_number = atoi(&inputBuffer[1]);
			strcpy(inputBuffer,history[command_number]);
			length = strlen(inputBuffer) + 1;
		}
	}
	
	addtohistory(inputBuffer);
	
	start = -1;
    for (i=0;i<length;i++) { 
        switch (inputBuffer[i]){
			case ' ' :               /* seperator */
				if(start != -1){
              		args[char_index] = &inputBuffer[start];    
		    		char_index++;
				}
				inputBuffer[i] = '\0'; /* add a null char; make a C string */
				start = -1;
				break;
			case '\n':                 //final char of args 
				if (start != -1){
					args[char_index] = &inputBuffer[start];     
		    		char_index++;
				}
				inputBuffer[i] = '\0';
				args[char_index] = NULL; /* no more arguments to this command */
				break;
				
	    	default :             /* some other character */
				if (start == -1)
		    		start = i;
              	if (inputBuffer[i] == '&') {
	    			*background  = 1;
                   	inputBuffer[i-1] = '\0';
				}
		} 
	}    
	
	if (*background)
		args[--char_index] = NULL;
	
	args[char_index] = NULL; /* just in case the input line was > 80 */
	
	
} 

int main(void)
{
	char inputBuffer[MAX_LINE]; 	
	int background;             	
	char *args[MAX_LINE/2 + 1];	
	pid_t child;            		/* process id of the child process */
	int shouldrun = 1;
	
	int i, upper;
		
    while (shouldrun){	//Program terminates normally inside setup
		background = 0;
		setup(inputBuffer,args,&background);	//next command
		if (strncmp(inputBuffer,"history", 7) == 0) {
			if (command_count < MAX_COMMANDS)
				upper = command_count;
			else 
				upper = MAX_COMMANDS;
			for (i = 0; i < upper; i++) {
				printf("%d \t %s\n", i, display_history[i]);
			}
			continue;
		}

				
		if (shouldrun) { //should run background process
			child = fork();      
			switch (child) {
				case 0: //child process
					execvp(args[0],args);
					break;
				default : // parent process
					if (background == 0) 
						while (child != wait(NULL));
			}
		}
    }
	
	return 0;
}



// #include <stdio.h>
// #include <string.h>
// #include <unistd.h>
// #define MAX_LINE 80 /* maximum length command */ 

// int main(void) 
// {  
// 	pid_t pid;
// 	char mychar[MAX_LINE];
//   	char arg[MAX_LINE];
// 	char arg_list[3][20]; //array of 2 stringsvector<int> hello
// 	//char* args[MAX_LINE/2 + 1]; /* command line arguments */ 
// 	int should_run = 1; /* flag to determine when to exit program */ 
// 	while (should_run)  
// 	{  
// 		printf("osh>"); 
// 		fflush(stdout);
// 		fscanf (stdin, "%s", mychar);
// 		fgets (arg, MAX_LINE, stdin);
// 		printf("Command entered: %s\n", mychar); 
// 		printf("Arguments entered: %s", arg);
// 		//char str[]= "- This is a sample string.";
// 		char * pch;
// 		pch  = strtok(arg, " \n");
// 		for(int i = 0; pch != NULL; i++){
// 			strcpy(arg_list[i], pch);
// 			printf("%s\n", pch);
// 			pch = strtok(NULL, " ");
// 		}
// 		pch=strchr(str,'\n');
		
// 		// getchar();
// 		// //execvp(char *command, char *params[]); 
// 		getchar();
// 		return 0;
// 		//execvp(char *command, char *params[]);
// 		/* flushes any buffers to the underlying file 
// 		* After reading user input, the steps are as follow
// 		s: 
// 		* (1) fork a child process  
// 		* (2) the child process will execute 
// 		* (3) if the command includes &, parent will wait 
// 		*/ 
// 	} 
// 	printf("\nPress ENTER key to Continue\n");
// 	getchar();
// 	return 0; 
// } 


