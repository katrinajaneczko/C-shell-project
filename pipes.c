// Katrina Janeczko
// 3207.003
// 3-23-2022
// Project 2: Developing a Linux Shell
// Module for dealing with pipes
// which redirect the standard output of the command on the left
// side of a pipe to the standard input of the command on the
// right side of the pipe

#include "myshell.h"

//returns number of programs separated by pipes in array of parsed_args
//if no pipe, return 0
//this return value corresponds with the num rows in progs
int num_piped(char **parsed_args) {
    int num = 0;
    for (int i=0; parsed_args[i] != NULL; ++i) {
        if (strcmp("|", parsed_args[i]) == 0) {
            num++; //+1 every time there's a pipe
        }
    }
    if (num>0) {
        return num+1;
    }
    return num;
}


/*
//for testing purposes for this module:
void print_arr(int rows, char *progs[rows][MAXARGS]) {
    for (int i=0; i<rows; ++i) {
        for (int j=0; progs[i][j] != NULL; ++j) {
            printf("[%d][%d]: %s ", i, j, progs[i][j]);
        }
        printf("\n");
    }
}
*/
void printrow(char **row) {
    for (int i=0; row[i] != NULL; i++) {
        printf("%s ", row[i]);
    }
    printf("\n");
}



int make_pipe(char **parsed_args, int len, char **envp) {
    
    int num_progs = num_piped(parsed_args); //num progs separated by pipes
    char *progs[num_progs][MAXARGS];
    int next_index = 0;

    //create array of arrays of args to store each program in a different row
    //where each program is separated by a pipe 
    int row=0;
    int col=0;
    for (int i=0; i<len; i++) {
        if (strcmp(parsed_args[i], "|") != 0)  {
            progs[row][col] = parsed_args[i];
            //FOR TESTING: printf("[%d][%d]%s\n", row, col, progs[row][col]);
            col++;
        }
        else {
            progs[row][col] = NULL;
            //FOR TESTING: printf("[%d][%d]%s\n", row, col, progs[row][col]);
            row++;
            if (i+1 < len) {
                col=0;
            }
        }
    }
    progs[row][col] = NULL;


    int prev, fds[2];
    prev = STDIN_FILENO;
    //create all child processes:
    int pid[num_progs]; //create array to hold all child PIDs
    int n = 0; //n is the current prog index 

    while (n < num_progs) { //last n = num_progs-1
        //Create pipe
        if (n < num_progs-1) { 
            if (pipe(fds) == -1){
                perror("failed to create pipe\n");
                return 1;
            }    
        }

        //Fork into child process n
        if ( (pid[n] = fork()) == -1 ) {
           printf("error making pipe for n=%d\n", n); 
           return 1;
        }

        //if child process
        if (pid[n] == 0) {
            //FOR TESTING: printf("pid of n=%d: %d\n", n, getpid());

            //Redirect previous pipe to stdin
            if (prev != STDIN_FILENO) {
                if ( dup2(prev, STDIN_FILENO) < 0 ) {
                    printf("error duplicating (read end) during n=%d\n", n); 
                    return 1;
                }
                close(prev); //close duplicate
            }
            
            if (n < num_progs-1) {
                //Redirect stdout to current pipe
                if ( dup2(fds[1], STDOUT_FILENO) < 0 ) {
                    printf("error duplicating (write end) during n=%d\n", n); 
                    return 1;
                }
                close(fds[1]); //close duplicate
            }
            
            //Start current command
            if (!builtin_cmd(progs[n], envp)) { 
                //execute prog n, which inherits open fds
                setenv("PARENT", getenv("SHELL"), 1);
                if ( execvp(progs[n][0], progs[n]) == -1 ) {
                    printf("error execing progs[%d]", n);
                    return 1;
                }
            } 
            exit(0);
            
        }

        //Close read end of previous pipe (not needed in parent)
        close(prev);
        //Close write end of current pipe (not needed in parent)
        close(fds[1]);
        //Save read end of current pipe to use in next iteration
        prev = fds[0];

        n++; //next prog
    }
    
    return pid[--n];
}
