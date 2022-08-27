#ifndef MYSHELL_DOT_H    /* This is an "include guard" */
#define MYSHELL_DOT_H    /* prevents the file from being included twice. */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <libgen.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

//constants
#define MAXLINE 1024 //max line size
#define MAXARGS 128 //max args on command line
#define MAXNAME 64 //max chars in hostname

//myshell and loop modules
void initialize(char *name, int interactive_mode);
void loop(char *name, int interactive_mode, FILE *inputstream, char **envp);
char* readln(FILE *inputstream);
int parse(char *line, char **parsed_args);
int eval(char **parsed_args, char **envp);
int isvalid(char **parsed_args);

//pipes module
int num_piped(char **parsed_args);
int make_pipe(char **parsed_args, int length, char **envp); 
void printrow(char **row);

//redirection module
int check_redirect_output(char **parsed_args);
int redirect_output(char **parsed_args);
int check_redirect_input(char **parsed_args);
int redirect_input(char **parsed_args);
int redirection(char **parsed_args);
int do_input_redirection(char **parsed_args, int in_index);
int do_output_redirection(char **parsed_args, int out_index);
    
//background module
int bg_check(char **parsed_args);
int makeleft(char **parsed_args, char **left_parsed_args, int index);
int makeright(char **parsed_args, char **right_parsed_args, int index);
int launch(char **parsed_args, char **envp);
int launch_process(int bg, int len, char **parsed_args, char **envp);
void launch_fg(char **parsed_args, int len, char **envp);
void launch_bg(char **parsed_args, int len, char **envp);

//builtin module:
int builtin_cmd(char **parsed_args, char **envp);
int num_builtins();
int cd(char **parsed_args, char **envp);
int clr(char **parsed_args, char **envp);
int dir(char **parsed_args, char **envp);
int environ(char **parsed_args, char **envp);
int path(char **parsed_args, char **envp);
int echo(char **parsed_args, char **envp);
int help(char **parsed_args, char **envp);
int pause_sh(char **parsed_args, char **envp);
int quit(char **parsed_args, char **envp);


#endif /* MYSHELL_DOT_H */
