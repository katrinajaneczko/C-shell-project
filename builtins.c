// Katrina Janeczko
// 3207.003
// 3-23-2022
// Project 2: Developing a Linux Shell
// Module for built-in commands

#include "myshell.h"


//List of builtin commands & corresponding functions
char *builtins_names[] = {
                    "cd",
                    "clr",
                    "dir",
                    "environ",
                    "path", /*do i need this?*/
                    "echo",
                    "help",
                    "pause",
                    "quit"
                    };

int (*builtins[]) (char **, char **) = {
                                &cd,
                                &clr,
                                &dir,
                                &environ,
                                &path,
                                &echo,
                                &help,
                                &pause_sh,
                                &quit

                                };

//utility function returns number of builtins 
int num_builtins() {
  return sizeof(builtins) / sizeof(char *);
}

//utility function to count arguments in parsed_args
int num_args(char **parsed_args) {
    int i;
    for (i=0; parsed_args[i] != NULL; ++i) {
    }
    return i;
}

//returns 1 if it IS a builtin, 0 (false) if not
//if is a builtin, runs it
int builtin_cmd(char **parsed_args, char **envp) {

    for (int i=0; i < num_builtins(); i++) {
        if (strcmp(parsed_args[0], builtins_names[i]) == 0) {
            return (*builtins[i])(parsed_args, envp); //run function
        }
    }
    return 0; //not builtin command
}

/*cd <directory> - Change the current default directory to  
<directory>. If the <directory> argument is not present, report 
the current directory. If the directory does not exist an 
appropriate error should be reported. This command should also 
change the PWD environment variable. */
int cd(char **parsed_args, char **envp){
    int err;
    char cwd[1024];
    int n = num_args(parsed_args);
    getcwd(cwd, sizeof(cwd));

    if (n > 2) {
        perror("cd");
        printf("Too many arguments");
    } else if (n == 1) {
        printf("Current directory is: %s\n", cwd);
    } else {
        if ( (err = chdir(parsed_args[1]) != 0) ) {
            if (errno == ENOENT) {
                strcat(cwd, parsed_args[1]);
                if ( (err = chdir(cwd) != 0) ) {
                    perror("cd");
                    printf("Not a valid directory.\n");
                }
            }
            
        }
    }
    return 1;
}

/*clr - Clear the screen.*/
int clr(char **parsed_args, char **envp) {
    printf("\033[H\033[J");
    return 1;
}

/*dir <directory> - List the contents of directory <directory>.*/
int dir(char **parsed_args, char **envp) {
    struct dirent *entry; //ptr to dir entry
    if (parsed_args[1] == NULL) {
        fprintf(stderr, "Command requires argument <directory>\n");
        return 1;
    }
    DIR *dir = opendir(parsed_args[1]);
    while ( (entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
    return 1;
}

/*environ - List all the environment strings. */
int environ(char **parsed_args, char **envp) {
    for (int i=0; envp[i] != NULL; i++) {
        printf("%s\n", envp[i]);
    }
    return 1;
}

/*path - takes 0 or more args, each separated by whitespace
if user sets path to empy, no progs can be run except builtins
path always overwrites old path with newly specified path*/
int path(char **parsed_args, char **envp) {
    char newpath[MAXLINE] = "";
    if (parsed_args[1] != NULL) {
        strcat(newpath, parsed_args[1]);
        for (int i=2; parsed_args[i] != NULL; ++i) {
            strcat(newpath, ":");
            strcat(newpath, parsed_args[i]);
        }
    }
    setenv("PATH", newpath, 1);
    printf("New Path: %s\n", strlen(getenv("PATH"))==0 ? "(empty)" : getenv("PATH"));
    return 1;
}

/*echo <comment> - Display  <comment> on the display followed by a 
new line (multiple spaces/tabs may be reduced to a single space). */
int echo(char **parsed_args, char **envp) {
    if (parsed_args[1] == NULL) {
        fprintf(stderr, "expected argument to \"echo\"\n");
    }
    else {
        for (int i=1; parsed_args[i] != NULL; ++i) {
            printf("%s ", parsed_args[i]);
        }
        printf("\n");
    }
    return 1;
}

/*help - Display the user manual using the more filter.  */
int help(char **parsed_args, char **envp) {
    pid_t pid;
    if ( (pid = fork()) == -1) {
        perror("fork"); 
    }
    else if (pid == 0) {
        char *path = getenv("README");
        setenv("PARENT", getenv("SHELL"), 1);
        if (execlp("more", "more", path, NULL) == -1) {
            perror("execl");
            exit(0);
        }
    }
    waitpid(pid, NULL, 0);
    return 1;
}

/*pause - Pause operation of the shell until 'Enter' is pressed. */
int pause_sh(char **parsed_args, char **envp) {
    int c;
    while (1) {
        //read a single character 
        c = fgetc(stdin); 
        if (c == 10) { 
            break; 
        }
    }
    return 1;
}

/* Quit the shell*/
int quit(char **parsed_args, char **envp) {
    if (parsed_args[1] != NULL) {
        perror("quit");
        printf("Command \"quit\" does not take any arguments.");
        return 1;
    }
    exit(0); 
}