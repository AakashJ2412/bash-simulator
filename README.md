# Bash Simulator 

##  Introduction
The shell has been coded in C, using both inbuilt system functions, and also some functionalities coded from scratch. It is supposed to mimic the behaviour of BASH terminal.
To use this shell, run "make" and then execute the shell file that is created by running "./shell", after assigning appropriate permissions to files if necessary.
##
##  Functionalities
All the given functionalities/requirements of the shell have been implemented in a modular fashion, emulating the BASH environment. This includes
1) Semi-colon separated list of commands
2) Support of '&' operator for background processes
3) Set current working directory as home
4) Implementation of echo, cd, pwd, ls, pinfo commands from scratch.
5) Support of all other commands by passing it to system with execvp(), in both foreground and background.
6) Store history of commands entered(upto 20).
7) Input/Redirection using <,>, and >>.
8) Command pipelines using |, and input/output priority over piping.
9) Creating, altering value, and deleting environment variables.
10) Displaying/terminating all current background processes.
11) Passing appropriate signal to job, implementation of fg and bg.
12) Implementation of CTRL + Z,C, and D, and "quit" statement.
13) Implementation of exit codes and symbolism.
##
##  File Structure

### head
All the headerfiles for the commands are stored in this folder. The files for each command are independent to each other, except "headers.h", which is common to all files and stores both the system header files, as well as any required global variables.

### source
All the source codes are stored in this folder. Each command has its own file, independent to each other. All of them are connected to main.c, which is where the program executes from. The files have various functions, that are called from int main(), in main.c, to execute the command, and then return.

###  history/makefile

history is the file that is used to store the history of commands passed. As the history should exist even when the program terminates, it is stored in a file. the makefile is used to generate the executable file to run the shell.
##
##  Command Implementation

###  echo
Syntax: `echo (String)`

This command is implemented by simply taking the string provided after "echo", filtering out unnecessary spaces, and outputting on stdout.

###   pwd
Syntax: `pwd`

This command uses getcwd() to obtain current working directory, and output on stdout.

###   cd
Syntax: `cd (dir_name)`

cd command switches the current working directory to the specified one. It supports only one argument, and can take both absolute and relative paths. It supports '.','..', '-' and '~' functionalities. chdir() command is used to switch directories, after verifying the number of arguments passed and formatting spaces.

###   ls
Syntax: `ls (attributes) (list_of_directories )`

ls command displays the contents and details of the specified directory. It works with absolute and relative paths, and supports '.', '..', and '~' notation. Currently, -l and -a attributes are supported, and any number of directories can be passed to ls. The dirent class of functionalities is used for this, such as opendir, closedir, and readdir. In addition, stat functionality is also required to read the details of each flag required for '-a' details.

###   pinfo
Syntax: `pinfo (pid)`

pinfo command displays the process details from the given process ID. If no ID is given, it assumes the current shell execution as process. It does this by reading details from the /proc/[pid]/stat file, and is thus exclusive to linux systems only.

###   history
Syntax: `history (number)`

history displays the required number of previously entered commands by the user, the number of which is specified by user. Each semicolon separated entry is considered separate. The max number of commands that can be stored is 20. The commands are stored in the "history" file, and are outputted from the file when required. This is done by writing the given command to a new file each time a command is entered, and copying at max 19 commands from "history" into the new file. Then, the old file is deleted and replaced by the new file.

###   background processes
Syntax: `(command) &`

Any command entered with an '&' is automatically considered as a background command, and is executed in the background after displaying its PID. If terminated, the user is notified whether it was done normally or abnormally. A linked list is used to store the PID and command of each process, which is called upon to notify termination. The background processes are implemented using fork() and signal handlers to catch the termination of child processes and reap any zombie processes.

###   setenv
Syntax: `setenv (var_name) (value)`

This command alters the value of the given variable name to the value, or creates a new environment variable with that name. In case no value is given, variable value is set as null. For incorrect number of arguments, error message is displayed. This is done with the use of setenv() function of C, after parsing the string and checking for errors.

###   unsetenv
Syntax: `unsetenv (var_name)`

This command destroys the environment variable corresponding to the given name. For incorrect number of arguments, error message is displayed. This is done with the use of unsetenv() function of C, after parsing the string and checking for errors.

###   jobs
Syntax: `jobs`

This command displays all the currently running background processes in order of their creation time. This is done by displaying all elements and their corresponding data stored in the Linked List that is specified in 'background processes'. The processes are automatically sorted in order of time, and job number is reset whenever a process is created or terminated. The process status is read from the /proc/[pid]/stat file.

###   kjob
Syntax: `kjob (job_no) (signal_number)`

This command sends the signal corresponding to the entered signal number to the process specified by the given job number. This is done by iterating through the linked list to specified process, and using kill() to send the required signal to its PID.

###   fg
Syntax: `fg (job_no)`

This command makes the specified running/stopped background process as a running process in the foreground. This is done by first ignoring the SIGTTOU and SIGTTIN signals for the current process, sending SIGCONT to the given job_no, and using tcsetpgrp() to bring the process to foreground. Once the process is finished, the shell is reassigned to the foreground and resetting SIGTTOU and SIGTTIN to their default values.

###   bg
Syntax: `fg (job_no)`

This command changes the specified background job from stopped to running. This is done by assigning SIGCONT to the PID of the given job, which is found by iterating through the linked list.

###   overkill
Syntax: `overkill`

This command terminates all the currently active background processes. This is done by iterating through the Linked list specified in 'background processes', and sending SIGKILL signal to all the stored process IDs.

###   quit
Syntax: `quit`

The shell terminates upon this command, but only does so after terminating any background processes that were running. Alternatively, CTRL + D can be passed on a null input line to exit the shell as well.
##
## functionalities

###   Input/Output Redirection

The shell supports input/output redirection with the use of <, >, and >> operators. It only reads from files if they exist, and files are created for output if they don't already exist. This is done with the use of dup2() operator, that changes the value of the default STDIN and STDOUT to the required file descriptors, before all normal operations are carried out.

###   Command Piping

Command piping is done with the use of both pipe() and fork(). In piping, the output of the first process becomes the input of the second process, and this process carries on. It is done by creating a pipe with old and new specifiers, iterating through each command of the pipe, and appropriately assigning, reassigning, and closing values for the pipe in parent and child. In addition, I/O redirection is given more priority to pipes, and thus in case of redirection, output/input of the pipe may be discontinued as file redirection is considered first.

###   Signal Handling

The shell provides signal handling for 2 commands.
CTRL + C is used to terminate any currently running foreground job, except the main shell program, and is done by sending SIGINT to the process.
CTRL + Z is used to stop any foreground process and send it to background. This is done by passing the SIGTSTP signal to it, and obtaining its PID and name from a previous save point to add to the linked list storing background processes.

###   Exit Codes

This is done by using a global variable that keeps track of whether an operation has encountered an error or not. In case any error is encountered, the value of the variable is set. The variable is checked while printing the prompt each time, and is reset at the end of each process.

## limitations

This project was done in a relatively short time-span, and thus has certain limitations:
1) All string lengths were considered under reasonable and normal usage, taking into consideration reducing redundancy in space utilization. It is easy to overflow these strings upon input, and although size can be increased fairly easily if required, it is requested to refrain from long commands.
2) Error handling has been implemented to a certain degree. However, there may be gaps present in the file which could lead to errors. In addition, the error messages outputted the shell may not match those produced in BASH.
3) In command piping, it is assumed that no process in the pipe stalls the command. This is erroneous as the next command of the pipe always waits for the previous process to terminate before continuing down the pipe. As such, if any process stalls during this, it will cause the pipe to stall. This can be resolved by replacing WUNTRACED in the parent process of piping with WNOHANG. However, while this automatically reaps stalled processes, it also triggers the signal handler of SIGCHLD which was used for our execution of background processes, resulting in multiple additional outputs.
4) Due to the asynchrous nature of signal handling and termination of background processes, there are often discrepencies in the way the prompt is printed, with there being a possibility of an extra prompt being printed due to abrupt termination of child processes/
5) Functionalities have been limited to our current requirements. Exceeding these requirements have been handled in many error cases, but may also lead to errors. 
6) There can be errors encountered during the combination of input/output redirection, piping, and background processes, due to the asynchronous nature of signal handling and multiple recursive forks being formed.