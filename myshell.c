// Katrina Janeczko
// 3207.003
// 3-23-2022
// Project 2: Developing a Linux Shell
// Main module, contains functions directly relating to main function

/* Referenced: 
    - https://www.geeksforgeeks.org/making-linux-shell-c/
    - Kerrisk, The Linux Programming Interface
    - Fiore class Powerpoint notes
*/ 

#include "myshell.h"

int main(int argc, char** argv, char **envp) {
    FILE *inputstream = NULL;

    // User or batch mode?
    int interactive_mode;
    if (argc == 1) {
        interactive_mode = 1;
        inputstream = stdin;
    } else if (argc > 2) {
        fprintf(stderr, "too many arguments.");
        printf("\nmyshell has 2 modes: interactive mode (no arguments) or batch mode (one argument, a .txt file).");
        exit(1);
    } else {
        interactive_mode = 0;
        // Check if batch source file exists
        if ( (inputstream = fopen(argv[1], "r")) == NULL) {
            perror("fopen");
            fprintf(stderr, "batch file cannot be opened.");
            fclose(inputstream);
            exit(1);
        }
    }

    // Get hostname for prompt
    char *name = (char*) malloc(MAXNAME * sizeof(char));
    gethostname(name, MAXNAME); 

    // Initialize shell
    initialize(name, interactive_mode);

    // Run an REPL (Read, Evaluate, Print Loop)
    loop(name, interactive_mode, inputstream, envp);
   
    // Cleanup
    fclose(inputstream);
    free(name);
    return 0;
}

// Initialize the shell for the user & give greeting
void initialize(char *name, int interactive_mode) {

    // Set SHELL environ variable
    char shell[MAXLINE] = "";
    strcat(shell, getenv("PWD"));
    strcat(shell, "/myshell");
    setenv("SHELL", shell, 1);

    // Set README environ vairable
    char readme[MAXLINE] = "";
    strcat(readme, getenv("PWD"));
    strcat(readme, "/readme_doc");
    setenv("README", readme, 1);

    //set PATH environ variable
    //setenv("PATH", "/bin", 1);

    // Clear the screen
    printf("\033[H\033[J");

    // Greeting
    printf("\n\n\n\n**********************************************");
    printf("\n\n\n\t******Hello %s******", name);
    printf("\n\t~ You are in %s ~", interactive_mode ? "Interactive Mode":" Batch Mode ");
    printf("\n\n\n\n**********************************************");
    printf("\n");

    sleep(1);
    
    // Clear the screen
    printf("\033[H\033[J");
}

// Run a REPL
void loop(char *name, int interactive_mode, FILE *inputstream, char **envp) {
    char *line; //string containing what user typed in cmdline
    char *parsed_args[MAXARGS]; //arrayof strings containing parsed cmdline
    int parsed_args_len;
    char cwd[MAXLINE];
    int bg_flag = 0;
    int status; //store value of status (success or error/exception?) when command executed
    int saved_stdout = dup(STDOUT_FILENO); //save fd of stdout
    int saved_stdin = dup(STDIN_FILENO); //save fd of stdin

    do {
        if (interactive_mode) {
            // Prompt
            getcwd(cwd, sizeof(cwd));
            fflush(stdout);
            waitpid(-1,NULL,WNOHANG); //harvest zombies
            printf("%s %s> ", name, basename(cwd));
            
        }

        // Read line from input
        line = readln(inputstream);

        // Parse commands, put into array of strings
        parse(line, parsed_args); //Ex: {["echo"],["hello"],["there"]}

        // Evaluate/execute the commands
        status = eval(parsed_args, envp);

        // Restore stdout/stdin, free memory
        fflush(stdout);
        dup2(saved_stdout, STDOUT_FILENO);
        dup2(saved_stdin, STDIN_FILENO);
        free(line);

    } while(status); // Continue if status=1 (success)

    // Close saved file descriptors
    close(saved_stdout);
    close(saved_stdin);
}


