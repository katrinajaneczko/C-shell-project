// Katrina Janeczko
// 3207.003
// 3-23-2022
// Project 2: Developing a Linux Shell
// Module for dealing with background execution with "&" symbol

#include "myshell.h"

//check if user wants it to be a background process
//returns index of first '&' flag, or -1 if not present 
int bg_check(char **parsed_args) {
    for (int i=0; parsed_args[i] != NULL; ++i) {
        if ( (strcmp(parsed_args[i], "&") == 0) ) {
            return i;        }
    }
    return -1;
}


int makeleft(char **parsed_args, char **left_parsed_args, int index) {
    int i;
    for (i=0; i<=index; ++i) {
        left_parsed_args[i] = parsed_args[i];
    }
    left_parsed_args[i] = NULL; //terminate
    return --i; //len
}

int makeright(char **parsed_args, char **right_parsed_args, int index) {
    int j = 0;
    for (int i=index+1; parsed_args[i] != NULL; ++i, ++j) {
        right_parsed_args[j] = parsed_args[i];
    }
    right_parsed_args[j] = NULL; //terminate
    return j; //len
}


int launch(char **parsed_args, char **envp) {
    char *left_parsed_args[MAXARGS];
    char *right_parsed_args[MAXARGS];
    int index;
    int lenleft;
    int lenright;
    pid_t pid;
    
    lenright = makeright(parsed_args, right_parsed_args, -1); //-1 so starts at 0

    //Begin loop for each additional background process (don't enter if no "&")
    int n = 1; //Indicates which iteration of the loop
    while ( (index = bg_check(right_parsed_args)) != -1 ) {

        //Make arrays from left and right sides of "&"
        lenleft = makeleft(right_parsed_args, left_parsed_args, index); 
        lenright = makeright(right_parsed_args, right_parsed_args, index);

        //Fork to create 2 shell processes
        if ( (pid = fork()) == -1) {
            perror("fork");
            return 1;
        }
        //If child, break
        else if (pid == 0) {
            break;
        }
        //Else if parent, continue loop
        ++n;
    }

    //If child (background process)
    if (pid == 0) {
        //Get rid of "&" at the end
        left_parsed_args[bg_check(left_parsed_args)] = NULL;
    
        //Launch in background
        return launch_process(n, lenleft, left_parsed_args, envp); 
        exit(0);//terminate
    }

    //Else, if parent and command still left, or if "&" was never included (fg process)
    else if (right_parsed_args[0] != NULL) {
        //Launch in foreground
        return launch_process(0, lenright, right_parsed_args, envp); 
        exit(0);//terminate
    }
    
    return 1;
}


//bg=# means & is included in the cmd (no wait)
//bg=0 means foreground process (wait)
int launch_process(int bg, int len, char **parsed_args, char **envp) {

    //Background process 
    if (bg) {
        printf("[%d]     %d    + ", bg, getpid());
        printrow(parsed_args);
        launch_bg(parsed_args, len, envp);
    }

    //Foreground process
    else {
        launch_fg(parsed_args, len, envp);
    }

    return 1;  //terminate
}

//Runs the necessary code to launch a background process (no waiting)
//(Note that child is already made if this is a background process)
void launch_bg(char **parsed_args, int len, char **envp) {
    pid_t pid;
    if (num_piped(parsed_args) != 0) {
        pid = make_pipe(parsed_args, len, envp);
    }
    else {
        //Check if builtin command (run if it is)
        if (!builtin_cmd(parsed_args, envp)) {
            setenv("PARENT", getenv("SHELL"), 1);
            //Exec to create new process
            if (execvp(parsed_args[0], parsed_args) == -1) { 
                perror("execvp");
                exit(0); //exit from failed process
            }
        }
    }
    exit(0); //No waiting since background process

}

//Runs the necessary code to launch a foreground process (waits for parent)
void launch_fg(char **parsed_args, int len, char **envp) {
    pid_t pid;
    if (num_piped(parsed_args) != 0) {
        pid = make_pipe(parsed_args, len, envp);
        //Since foreground process, wait for child
        waitpid(pid, NULL, 0);
    }
    else {
        //Check if builtin command (run if it is)
        if (!builtin_cmd(parsed_args, envp)) { 
            //If not builtin...
            //Fork process and save return value (pid)
            if ( (pid = fork()) == -1) {
                perror("fork"); //error forking
            }
            //If child...
            else if (pid == 0) {
                setenv("PARENT", getenv("SHELL"), 1);
                //Exec to create new process
                if (execvp(parsed_args[0], parsed_args) == -1) { 
                    perror("execvp");
                    exit(0); //exit from failed process
                }
            }
            //Else, if parent...
            else { 
                //Since foreground process, wait for child
                waitpid(pid, NULL, 0);
            }
        }
    }
}