// Katrina Janeczko
// 3207.003
// 3-23-2022
// Project 2: Developing a Linux Shell
// Module for functions directly called in REPL

#include "myshell.h"

// Reads command line typed by user into string
char *readln(FILE *inputstream) {
    char *line = NULL; //since set to NULL before call, getline() will allocate buffer for storing the line
    size_t bufsize = 0; // getline will allocate a buffer for us

    //Read input line by line
    if (getline(&line, &bufsize, inputstream) == -1) {
        if (feof(inputstream)) {
            exit(0); //EOF received (if done batchfile or ctrl+D) so exit success
        }
        else {
            perror("readln");
            exit(1); //error occurred so exit fail
        }
    }
    return line;
}

// Parse command line string into an array of arguments
int parse(char *line, char **parsed_args) {
    int i; //arg index/counter

    for (i=0; i < MAXARGS; i++) {
        //strsep finds first token delimited by delimiter in line
        //replace first delimiter (space, tab, nelwine) with '\0'
        //make this token the first entry of array parsed_args
        parsed_args[i] = strsep(&line, " \n\t"); 
        //FOR TESTING: printf("parsed_args[%d]: %s\n", i, parsed_args[i]);
  
        //keep going till end of string line
        if (parsed_args[i] == NULL) {
            break;
        }
        //ignore spaces, don't include as valid tokens
        if (strlen(parsed_args[i]) == 0)
            i--;
    }
    return i; //returns 0 if parsed_args is empty, or nonzero (num args) if filled
}


// Evaluate and execute the commands
int eval(char **parsed_args, char **envp) {
    int i;
    
    // Check if user entered empty command
    if (parsed_args[0] == NULL) {
        return 1; //return status=1, restart loop
    }

    // Check for validity of input
    if (!isvalid(parsed_args)) {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }

    // Check for redirection / perform if neededs
    if (!redirection(parsed_args)) {
        //error redirecting
        return 1;
        printf("here: oof \n");
    }
    
    // Launch commands (fg/bg capabilities, pipes included)
    return launch(parsed_args, envp);
    
    return 1;
}


// Returns 1 if command line input is valid, 0 if not
int isvalid(char **parsed_args) {
    int i;
    int count_input_red=0;
    int count_output_red1=0;
    int count_output_red2=0;

    for (i=0; parsed_args[i]; ++i) {
        if (strcmp(parsed_args[i], "<")==0) {
            count_input_red++;
        } else if (strcmp(parsed_args[i], ">")==0) {
            count_output_red1++;
        } else if (strcmp(parsed_args[i], ">>")==0) {
            count_output_red2++;
        }
        
        if (strcmp(parsed_args[i], "|")==0 && strcmp(parsed_args[i+1], "&")==0) {
            return 0;
        }
    }

    if (count_input_red>1 || count_output_red1>1 || count_output_red2>1) {
      return 0;
    }

    return 1; //return 1 if valid
}

