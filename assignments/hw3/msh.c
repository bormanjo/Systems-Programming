#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <signal.h>

int debug = 0;
int MAX_LEN = 1000;

// run_fg - runs a command and arguments in the fg via fork
int run_fg(char *argv[], int argc){
	// process variables
	pid_t pid;	// The pid and status of the child process
	int stat;
	
	// signal handling
	sigset_t mask_all, mask_one, prev_one;
	
	// sigfillset(&mask_all);
	// sigemptyset(&mask_one);
	// sigaddset(&mask_one, SIGCHLD);
	
	// copy arguments from argv to argv_new, 
	char *argv_new[argc-1];
	for(int i = 1; i < argc; i++){	// First argument is the command, exclude it	
		argv_new[i-1] = argv[i];
	}
	
	// sigprocmask(SIG_BLOCK, &mask_one, &prev_one); // Block SIGCHLD
	if ((pid = fork()) == 0){
		// Execute command and arguments
		if (debug) printf("Child process: Executing in foreground\n");
		
		// Unblock SIGCHLD
		// sigprocmask(SIG_SETMASK, &prev_one, NULL);	// Unblock SIGCHLD
		
		// Exec (cmd, args);
		execv(argv[0], argv_new);
	} else {
		if (debug) printf("Parent process: Waiting for child\n");
		
		// Block all
		// sigprocmask(SIG_BLOCK, &mask_all, NULL);
		
		waitpid(pid, &stat, 0);
	}
	
	return 0;
}

// run_bg - runs a command and arguments in the bg via exec
int run_bg(char *argv[], int argc){
	pid_t pid;	// The pid and status of the child process
	int stat;
	
	// copy arguments from argv to argv_new, 
	char *argv_new[argc-2];
	for(int i = 1; i < argc - 1; i++){	// First argument is the command, exclude it	
		argv_new[i-1] = argv[i];		// Last argument is the '&' bg identifier, exclude it
	}
	
	if ((pid = fork()) == 0){
		if (debug) printf("Child process: Executing in background");
		printf("pid: %d cmd: %s", argv[0]);
		
		// Exec (cmd, args);
		execv(argv[0], argv_new);
	} else {
		waitpid(pid, &stat, WNOHANG);
	}
	
	return 0;
}



// msh
int msh(){
	// process variables
	pid_t wpid;
	int stat = 0, i = 0;
	struct passwd *pw;
	char directory[MAX_LEN], prompt[MAX_LEN], istr[MAX_LEN];
	char sep = ' ', *tok[50];
	
	// Get username
	pw = getpwuid(getuid());
	char username = pw->pw_name;
	
	initjobs();
	
	while(1){
		// Get current directory
		if (getcwd(directory, sizeof(directory)) == NULL){
			perror("getcwd() error");
			return -1;
		}
		
		// Get prompt
		sprintf(prompt, "SWS:%s:%s>", username, directory); // prompt = "SWS:username:current directory>"
			
		// Get input string as istring
		fgets(istr, MAX_LEN, stdin);
		
		// Process istring
		tok[0] = strtok(istr, sep);		// Tokenize istring
		
		while(tok[i] != NULL){
			i++;
			tok[i] = strtok(NULL, sep);
		}	
			
		if (debug) printf("First token: %s", tok[0]);
			
		// Check first argument
		if (tok[0] == "cd"){
			// change directory to remaining command arguments
			chdir(tok[1]);
		} else if (tok[0] == "exit"){
			break;	// BREAK LOOP
		} else {
			// Check last argument:
			if(tok[i] == '&'){
				// exec in bg
				run_bg(&tok, i);
			} else {
				// exec in fg
				run_fg(&tok, i);
			}
			
		}

	}
	
	// Wait for bg processes
	while ((wpid = wait(&stat)) > 0){
		
	}
	
	exit(0);
}


//cp_handler - Child Process Handler, handles SIGCHLD when a child process exits
void cp_handler(int sig){
	pid_t pid;
	
	while((pid = wait(NULL)) > 0){
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