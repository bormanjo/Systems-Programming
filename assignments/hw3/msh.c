#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <signal.h>
#include <string.h>
#include "csapp.h"

int debug = 1;
int MAX_LEN = 1000;

// run_fg - runs a command and arguments in the fg via fork
int run_fg(char **argv, int argc){

	pid_t pid;	// The pid and status of the child process
	int stat;

	if ((pid = fork()) == 0){
		// Execute command and arguments
		if (debug) printf("Child process: Executing in foreground\n");
		
		// Exec (cmd, args);
		execvp(argv[0], argv);
		
		sleep(2);
		
		exit(0);
		
	} else if (pid > 0) {
		if (debug) printf("Parent process: Waiting for child\n");
		
		wait(NULL);
		
	} else {
		perror("fork failure");
		return -1;
	}
	
	return 0;
}

// run_bg - runs a command and arguments in the bg via exec
int run_bg(char **argv, int argc){
	
	pid_t pid;	// The pid and status of the child process
	int stat;
	
	if ((pid = fork()) == 0){
		
		if (debug) printf("Child process: Executing in background\n");
		
		printf("pid: %d cmd: ", getpid());
		
		for(int i = 0; i < argc; i++){
			printf("%s ", argv[i]);
		}
		
		printf("\n");
		
		// Exec (cmd, args);
		execvp(argv[0], argv);
		
		sleep(2);
		
		exit(0);
		
	} else if (pid > 0){
		
		if (debug) printf("Parent process: Waiting WNOHANG\n");
		waitpid(pid, &stat, WNOHANG);
		
	} else {
		perror("fork failure");
		return -1;
	}
	
	return 0;
}

int parse_line(char *istr, char **argv){
	int i = 0, len;		// Keeps track of the current idx
	
	if ((len = strcspn(istr, "\n")) == 0){
		return -1;
	}		
	
	istr[len] = 0;	// Remove newline (due to fgets)
	
	argv[i] = strtok(istr, " "); 	// Parse the first argument
	
	while(argv[i] != NULL){					// While arguments are parsed
		argv[++i] = strtok(NULL, " "); 		// Store the argument at the next i
	}
	
	if (debug){	// Print the stored commands
		printf("Parsed args:\n");
		for(int j = 0; j < i; j++){
			printf("\t%s\n", argv[j]);
		}
	}
	
	return i - 1;
}

// msh
int msh(){
	// process variables
	pid_t wpid;
	int argc, stat = 0;
	struct passwd *pw;
	char directory[MAX_LEN], istr[MAX_LEN];
	char *argv[100];
	int running = 1;

		
	// Get username
	pw = getpwuid(getuid());
	
	while(running){
		// Get current directory
		if (getcwd(directory, sizeof(directory)) == NULL){
			perror("getcwd() error");
			return -1;
		}
		
		// Get prompt
		printf("SWS:%s:%s>", pw->pw_name, directory); // prompt = "SWS:username:current directory>"
		
		// Get input string as istring
		fgets(istr, MAX_LEN, stdin);
		
		if (debug) printf("Input String: %s\n", istr);
		
		argc = parse_line(istr, argv);	
		
		if (argc >= 0){
		
			if (debug) printf("First token: %s\n", argv[0]);
			
			// Check first argument
			if (strcmp(argv[0], "cd") == 0){
				// change directory to remaining command arguments
				if (argc >= 1){
					if (debug) printf("Change directory to: %s\n", argv[1]);
					
					chdir(argv[1]);
				}
				
			} else if (strcmp(argv[0], "exit") == 0){
				
				if (debug) printf("Exiting shell\n");
				
				killpg(getpid(), SIGTERM);

				running = 0;	// BREAK LOOP
			} else {
				printf("here2\n");
				
				// Check last argument:
				if(strcmp(argv[argc], "&") == 0){
					
					if (debug) printf("Running in BG\n");
					
					argv[argc] = NULL;
					
					// exec in bg
					run_bg(argv, argc);
				} else {
					
					if (debug) printf("Running in FG\n");
					
					// exec in fg
					run_fg(argv, argc);
				}
				
			}
		
		}
		
		while((wpid = waitpid(-1, 0, WNOHANG)) > 0){
			sio_puts("pid: ");
			sio_putl((long) wpid);
			sio_puts(" done\n");
		}
		
	}
	
	return 0;
}


//cp_handler - Child Process Handler, handles SIGCHLD when a child process exits
void cp_handler(int sig){
	pid_t pid;
	
	while((pid = waitpid(-1, 0, WNOHANG)) > 0){
		sio_puts("pid: ");
		sio_putl((long) pid);
		sio_puts(" done\n");
	}
}

void sigint_handler(int sig){
	printf("\n");
}

// main
int main(int argc, char *argv[]){
	// Check inputs
	if (argc != 1){
		perror("Usage: msh");
		return -1;
	}
	
	// Install Signal Handlers
	signal(SIGCHLD, cp_handler);
	signal(SIGINT, sigint_handler);
	
	// Run msh function
	msh();
	
	// Exit
	return 0;
}