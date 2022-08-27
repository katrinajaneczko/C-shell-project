# PROJECT 2: myshell
Katrina Janeczko <br>3207.003 <br>3-23-2022 <br>Project-2-Sp22: Developing a Linux Shell 

## DESIGN DESCRIPTION

### INTRODUCTION

This project is a simple version of a Unix/Linux shell with with I/O redirection capabilities, support of pipes, and support of 
background program execution. This shell may be invoked with no command line arguments, which will present it in Interactive Mode, 
or with a batchfile name as the argument, which will run the shell in Batch Mode and then exit the shell immediately after.

The program implements the features below triggered by the following symbols:

|  &nbsp;&nbsp;     OPTION &nbsp;&nbsp;		| 	DESCRIPTION   |  
| ------------- | ------------------- |
| &  | Background execution of programs. An ampersand (&) at the end of the command line causes the shell to return to the command line prompt immediately after launching the program preceding it. The shell supports multiple background program commands in a row in order to achieve parallel commands.  |  	
| < | Redirects stdin to be from the file specified on the right side of '<'.  |
| > | Redirects stdout to be the file specified on the right side of '>'. If the file does not already exist, it creates it, otherwise it overwrites it.  |
| >> | Redirects stdout to be the file specified on the right side of '>>'. If the file does not already exist, it creates it, otherwise it appends to it.  |
| \| | Implementation of command line pipes, which are essentially an extension of redirection, so that commands can be strung together. An example is ‘prog1 arg1 | prog2 | prog3’, where the output of prog1 would be redirected to the input of prog2, and the output of prog2 would be redirected as the input of prog3. |


### BUILT-IN COMMANDS

The shell implements built-in commands as follows:

|  &nbsp;&nbsp;     COMMAND &nbsp;&nbsp;		| 	DESCRIPTION   |  
| ------------- | ------------------- |
| cd \<directory\>  | Change the current default directory to  <directory>. If the <directory> argument is not present, report the current directory. This command also changes the PWD environment variable.  |  	
| clr | Clear the screen.   |
| dir \<directory\> | List the contents of directory <directory>.   |
| environ | List all the environment strings.   |
| path \<path\> | Takes 0 or more arguments, with each argument separated by whitespace from the others. A typical usage would be like this: myshell> path /bin /usr/bin, which would add /bin and /usr/bin to the search path of the shell. If the user sets path to be empty, then the shell cannot run any programs (except built-in commands). The path command always overwrites the old path with the newly specified path.  |
| echo \<comment\> | Display  <comment> on the display followed by a new line (multiple spaces/tabs reduced to a single space).    |
| help | Display the user manual using the more filter.    |
| pause | Pause operation of the shell until 'Enter' is pressed.   |
| quit | Quit the shell.   |


### BUILDING INSTRUCTIONS
One may compile the project by typing `make` in order to trigger the makefile to compile and link each component of the code. 

Then, as an example, type a command such as `./myshell` to invoke the program in Interactive Mode, where user input is read from the command line in the terminal, or `./myshell batchfile` to invoke the program in Batch Mode, where input is read from a file.


### DESIGN OVERVIEW

**makefile**: <br>
  Type `make` in the command line to build and compile the entire program. <br>
  Type `make clean` to invoke the clean rule that removes .o files and executables. 


**myshell.h**: <br>
  Header file for all .c files in the project. <br>
  Contains #include statements for external libraries. <br>
  Contains all function prototypes, separated by module by comments. <br>
  Each of the other .c files import this header file. <br>


**myshell.c**: <br>
This module contains the following function definitions (and is the driver program of myshell): <br>
int main(int argc, char** argv, char **envp); <br>
void initialize(char *name, int interactive_mode); <br>
void loop(char *name, int interactive_mode, FILE *inputstream, char **envp); <br>
  
In main(), the input stream is defined, which will either be from the batchfile specified or from stdin depending on what was originally entered when running myshell. Then initialize() will initialize environment variables like SHELL and README as well as print a greeting. The hostname is acquired in order to print the shell prompt when in Interactive Mode also. Then the function loop() will run a read-evaluate-print loop, whose components are specified more in the loop module. Then streams are closed and memory is freed. <br>
  
In the loop() function, we first duplicate the current stdin and stdout for use in redirection later. The prompt is printed to the user to the temrinal if in Interactive Mode, then input is read, then the line is parsed into the array of strings char **parsed_args, which is a very important variable throughout the program and is used and manipulated many times. eval() is called, returning status of 0 or 1 (where 1 indicates success and 0 failure). While status=1, we continue the loop. Afterward, we make sure to close the file descriptors and free memory and flush stdout. <br>

**loop.c**: <br>
This module contains the following function definitions, which are directly related to the REPL in main(): <br>
char* readln(FILE *inputstream); <br>
int parse(char *line, char **parsed_args); <br>
int eval(char **parsed_args, char **envp); <br>
int isvalid(char **parsed_args); <br>
  
readln() takes in the commandline typed in by the user and reads it into a string which is output. Then parse() is called on this line in order to create from the string an array of arguments called parsed_args. It returns 0 if parsed_args is empty, or nonzero if filled. parsed_args is truncated by placing NULL manually in the last position after all the arguments which fill it. In the eval() function, isvalid() makes sure the input is valid, such as making sure the < symbol is not there twice or a & does not immediately follow a | character because these inputs would not have behavior that has been defined (nor does an actual shell). When an error is thrown, the shell prints an error message but continues prompting, just like an actual shell. Now, the presence of redirection characters are checked for, and if present, then the redirection functions are called, which are in the redirection module. Then of course the commands are launched with the launch() function, specified in the background module. <br>
  
**background.c**: <br>
This mdule is for dealing with background execution with "&" symbol. The functions it contains are: <br>
int bg_check(char **parsed_args); <br>
int makeleft(char **parsed_args, char **left_parsed_args, int index);<br>
int makeright(char **parsed_args, char **right_parsed_args, int index);<br>
int launch(char **parsed_args, char **envp);<br>
int launch_process(int bg, int len, char **parsed_args, char **envp);<br>
void launch_fg(char **parsed_args, int len, char **envp);<br>
void launch_bg(char **parsed_args, int len, char **envp);<br>

The principal function here is launch(), which splits parsed_args into a left and right side if a '&' is present, over and over in a while loop. It also works for just one process without the '&'. If '&' is present, we fork() to create 2 shell processes, if we are the child we break from the loop, truncate the arguments, and call launch_process(). The parent does not wait in the case of a background process. If not paired with '&', then the process will also be launched with launch_process() but in that function, wait is called since it is a foreground process. Otherwise in that function, for a background process we call specifically launch_bg() and for a foreground process we call launch_fg(), which deal respectively with running the actual commands, first checking if it has pipes (and if it does then sending it through make_pipe(), which is explained in the pipes module more), then checking if it is builtin and if it isn't then calling execvp() to create a new process. We make sure to return or exit from child processes and reap the zombies at some point. This is done by calling wait() or waitpid().<br>
  
**pipes.c**: <br>
This module is for dealing with pipes, which redirect the standard output of the command on the left side of a pipe to the standard input of the command on the right side of the pipe. The functions in this module are: <br>
int num_piped(char **parsed_args);<br>
int make_pipe(char **parsed_args, int length, char **envp); <br>
void printrow(char **row); <br>
  
We find the number of pipes (or if there are any at all) using the num_piped() function, which returns the number of programs separated by pipes in the array of parsed_args. Then, if there are pipes present, make_pipe() is entered, and immediately an array of arrays is created where each row is its own program separated by a pipe. Then a while loop is entered which continues to create pipes as long as there are any, but it also works with just a singular process. Input and output is redirected appropriately using pipe() and an array of file descriptors corresponding to the read and write sides of the pipe. If the command is the first in a series of pipes commands, then input will be stdin, and if not, it is redirected. If not the last command in a string of commands separated by pipes, then output is redirected as explained. The result is that multiple processes can be strung together as desired. A simple printrow() function is also found in this module which simply allows for testing or printing to the user a row of the double array. <br>
  

**redirection.c**: <br>
This is the module for dealing with I/O redirection which is the transfer of standard output to some other destination or using some other input besides standard input. The functions in this module are as follows:
int check_redirect_output(char **parsed_args); <br>
int redirect_output(char **parsed_args);<br>
int check_redirect_input(char **parsed_args);<br>
int redirect_input(char **parsed_args);<br>
int redirection(char **parsed_args);<br>
int do_input_redirection(char **parsed_args, int in_index);<br>
int do_output_redirection(char **parsed_args, int out_index);<br>
 
From the eval() function, redirection() is called, which checks for the presence of redirecion characters, then decides which order to deal with input vs output redirection depending on the order they were input by the user. In do_output_redirection(), redirect_output() uses the system call open() or creat() depending on whether '>' or '>>' was chosen by the user, and dup2() is used to redirect the stdout to the file input by the user instead of the terminal. Similarly, in do_input_redirection(), redirect_input() uses open() and dup2() to redirect stdin from the terminal (or batchfile) to from whatever file is specified. The stdin and stdout file descriptors will be reset each iteration fo the main REPL loop so that does not need to be done here.<br>

  
**builtins.c**: <br>
This module simply holds all the builtin commands functions that the user can call straight from the shell without using execvp(). These are the functions in the module:
int builtin_cmd(char **parsed_args, char **envp); <br>
int num_builtins();<br>
int cd(char **parsed_args, char **envp);<br>
int clr(char **parsed_args, char **envp);<br>
int dir(char **parsed_args, char **envp);<br>
int environ(char **parsed_args, char **envp);<br>
int path(char **parsed_args, char **envp);<br>
int echo(char **parsed_args, char **envp);<br>
int help(char **parsed_args, char **envp);<br>
int pause_sh(char **parsed_args, char **envp);<br>
int quit(char **parsed_args, char **envp);<br>

Note that char *builtins_names[] is a list of the builtin commands & corresponding functions, so that I may call them in builtin_cmd() using an array of pointers to functions. Also, num_builtins() is just a utility function returning the number of these functions in case I add to the shell's builtin functionality later. The commands all have the behaviors that are specified in the table above. <br>

  

## TESTING, DISCUSSION, ISSUES, RESULTS
To test my shell, I created a folder in my project directory called “demo”, and in this folder I created some files called input.txt (a sample text input file), copycat.c (a sample C program that just prints back whatever was entered), and hello.c (a simple hello world program). Then, I wrote a batchfile called batch.txt which is located in the demo folder. First, I ran each of the commands listed in this file individually so as to test if Interactive Mode was working with each command, and then I ran myshell with the input of batch.txt to test Batch Mode. The commands attempted are below: 
<br><img width="377" alt="Screen Shot 2022-03-23 at 3 22 23 AM" src="https://user-images.githubusercontent.com/73796086/159645197-96ed32db-3f75-4ca6-954f-fc62214bd343.png"><br>
Each of the commands was able to complete successfully in the end. However, throughout testing, I found that many commands which should have worked were having issues such as segmentation faults or just general incorrect performance. It took many hours to properly test and debug using GDB and find the issues then correct them. I will also not that I was testing my code incrementally as I coded, because it never worked right the first try, but that's why it was so important to test each piece of each function individually before moving on. After making sure all valid inputs worked, I created another batchfile called batcherrors in the demo folder in order to test if the behavior was giving an error when it should be. Note that throughout the process of coding this lab, I implemented checks (such as making sure fork() did not return -1 or open() worked correctly). Here are some other non-obvious examples that throw errors:
<br><img width="192" alt="Screen Shot 2022-03-23 at 3 26 43 AM" src="https://user-images.githubusercontent.com/73796086/159645799-44bc52d7-7e53-44d8-8491-9fe55a0e5aef.png"><br>
  I have found that the only command I have tried that does not work properly is `vim &` which opens the program and the user is unable to exit and continue using the shell. This has to do with specifically the way I am handling background processes which is simply not calling `wait()` in the parent, however there is another way to implement background or concurrent processes which will allow for this command to work, but it was not required so that will be left for another day. <br>
My shell's behaviors are very similar except when it comes to background processes for the reason mentioned above. If I am to work on this lab more, I want to implement this functionality. I was able to implement multiple pipes and multiple concurrent processes much like an actual shell.