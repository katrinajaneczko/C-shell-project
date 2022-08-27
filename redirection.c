// Katrina Janeczko
// 3207.003
// 3-23-2022
// Project 2: Developing a Linux Shell
// Module for dealing with I/O redirection
// which is the transfer of standard output to some other destination
// or using some other input besides standard input

#include "myshell.h"

/* Referenced:
   https://man7.org/linux/man-pages/man2/open.2.html
*/

//Deal with input redirection: prog1 < some_input_file
//prog1's stdin fd is changed to some_input_file
int redirect_input (char **parsed_args) {
    int i = check_redirect_input(parsed_args);
    char *input_filename = parsed_args[i+1];
    int input_fd = open(input_filename, O_RDONLY, 0777);

    if (input_fd == -1) {
        fprintf(stderr, "open error \n");
        return 1;
    }

    //Redirect stdin to be from input_fd
    if ( dup2(input_fd, STDIN_FILENO) < 0 ) {
        printf("dup2 error\n"); 
        return 1;
    }
    close(input_fd); //close duplicate fd
    return 0; 
}

//Deal with output redirection: prog1 > some_output_file
//prog1's stdout fd is changed to some_output_file
int redirect_output (char **parsed_args) {
    int i = check_redirect_output(parsed_args);
    int output_fd;
    //char *command = parsed_args[i-1]; //this could be ls -la though... not just 1 string...
    char *output_filename = parsed_args[i+1];

    if (strcmp(parsed_args[i], ">") == 0) {
        //creates new if doesn't exist, or overwrites (same as open() with O_WRONLY | O_CREAT | O_TRUNC)
        output_fd = creat(output_filename, 0777);
    } else {
        //creates new if doesn't exist, or appends
        output_fd = open(output_filename, O_WRONLY | O_CREAT | O_APPEND, 0777);
    }
    
    if (output_fd == -1) {
        fprintf(stderr, "open error\n");
        return 1;
    }

    //Redirect stdout to output_fd
    if ( dup2(output_fd, STDOUT_FILENO) < 0 ) {
        printf("dup2 error\n"); 
        return 1;
    }
    close(output_fd); //close duplicate fd
    return 0;
}

//return index of > or >> (or -1 if not present)
int check_redirect_output(char **parsed_args) {
    for (int i=1; parsed_args[i] != NULL; ++i) {
        if ( (strcmp(parsed_args[i], ">") == 0) || (strcmp(parsed_args[i], ">>") == 0) ) {
            return i;
        }
    }
    return -1;
} 

//return index of <, or -1 if not present
int check_redirect_input(char **parsed_args) {
    for (int i=1; parsed_args[i] != NULL; ++i) {
        if ( (strcmp(parsed_args[i], "<") == 0) ) {
            return i;
        }
    }
    return -1;
}


int redirection(char **parsed_args) {
    int in = check_redirect_input(parsed_args);
    int out = check_redirect_output(parsed_args);
    // If no redirection present
    if (in == -1 && out == -1) {
        return 1;
    }

    // If both redirections present
    if (in != -1 && out != -1) {
        // if out first
        if (in>out) {
            if (!do_input_redirection(parsed_args, in)) {
                return 0;
            }
            if (!do_output_redirection(parsed_args, out)) {
                return 0;
            }
        } else { // in first
            if (!do_output_redirection(parsed_args, out)) {
                return 0;
            }
            if (!do_input_redirection(parsed_args, in)) {
                return 0;
            }
        }
    }

    // If in is present, not out
    else if (in != -1) {
        if (!do_input_redirection(parsed_args, in)) {
            return 0;
        }
    }

    // If out is present, not in
    else if (out != -1) {
        if (!do_output_redirection(parsed_args, out)) {
            return 0;
        }
    }
        
    return 1;
}

// Check if "<" included; if index not -1, then redirect
int do_input_redirection(char **parsed_args, int in_index) {
    if (redirect_input(parsed_args)) {
        fprintf(stderr, "Could not redirect input\n");
        return 0;
    }
    parsed_args[in_index] = NULL; //truncate "< input.txt"
    return 1;
}

// Check if ">"/">>" included; if index not -1, then redirect
int do_output_redirection(char **parsed_args, int out_index) {
    if (redirect_output(parsed_args)) {
        fprintf(stderr, "Could not redirect output\n");
        return 0;
    }
    parsed_args[out_index] = NULL; //truncate "> output.txt"
    return 1;
}
    