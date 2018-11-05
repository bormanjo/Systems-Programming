
Objective:
	- Create a simple shell called "msh" that launches programs using exec() and fork()
	- Be able to
		- Create a prompt and accept a string from the keyboard
		- Process the string and decide:
			i). Is it a built in command? (cd, exit)
			ii). Launch in foreground?
			iii). Launch in background?
		- Do not need to check correctness of the shell inputs
			- Launch user's commands and let OS handle errors

Requirements:
	X. Prompt has form:
		- "SWS:username:directory>"
			- "SWS" is a fixed identifier
			- "username" - retrieve from password file using the user's uid (getpwuid())
			- "directory" - retrieve the current working directory
			
			=> See the 3rd set of notes
	X. Tokenize the input string via strtok()
		- Assume that commands, options and arguments are separated by spaces
	X. Built-in commands:
		- cd and exit
			Execute WITHOUT forking a new process
	X. Non-Built-in commands:
		- if '&' is at the end of an input string, the process should be executed in the background
		- Ensure there are NO ZOMBIES
			- Before process starts in bg, print the PID number and the input string:
				=> pid: 1366 cmd: ks
			- After the child process is reaped, print its pid again:
				=> pid 1366 done
	X. Foreground execution:
		- A child process should be forked and the parent process should wait for it. 
		- No printouts should appear
	X. Install signal handlers for SIGINT and SIGCHLD using signal()
		- SIGINT should be disabled for the parent process
			- print '\n' when received
			- Shell SHOULD NOT BE TERMINATED
		- SIGINT is restored after exec() is called	
			- ONLY worry about the parent process
		- See 10th set of Notes for SIGCHLD
		
Hint:
	- See 9th set of notes for pseudo code
	- include:
		- unistd.h
		- signal.h
		- sys/signal.h
	- if trouble with username, use uid instead
	- Remove '&' from the arguments passed to command that will be launched
	- use waitpid() to wait for children in foreground
	- use waitpid() with WNOHANG to reap child processes in the background
	- start with either foreground or background
	- use exit to exit, not CTRL-C
	
Deliverables:
	- Source files w/ at least one header
	- Maakefile
	- PDF file with answers to:
		a). Why are 'exit' and 'cd' handled as built-in commands and no child processes are forked? What would happen if you forked children for these commands?
			- cd - on the child would have its directory changed but not the parent. 
				 - on the parent would have its directory changed as well as all inheriting children
			- exit - on the child would kill the child instantly and nothing would happen to the parent
				   - on the parent would kill the parent process, as intended
		b). Why should you use killpg(getpid(), SIGTERM); before main() ends?
			- killpg sends the SIGTERM termination signal to the entire process group (hence pg). Therefore it prevents any potentailly lingering processes from remaining after the spawning (parent) process terminates
