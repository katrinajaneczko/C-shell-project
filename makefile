# Katrina Janeczko
# 3207.003
# 3-23-2022
# Project 2: Developing a Linux Shell

# define compiler as gcc
CC = gcc

#compiler flags
C  = -I -Wall -Werror

# typing 'make' will invoke the first target entry in the file (in this case the default target entry)
default: myshell

myshell:  myshell.o loop.o builtins.o pipes.o redirection.o background.o
	$(CC) $(CFLAGS) -o myshell myshell.o loop.o builtins.o pipes.o redirection.o background.o

myshell.o: myshell.c
	$(CC) $(CFLAGS) -c myshell.c

loop.o: loop.c
	$(CC) $(CFLAGS) -c loop.c

builtins.o: builtins.c
	$(CC) $(CFLAGS) -c builtins.c

pipes.o: pipes.c
	$(CC) $(CFLAGS) -c pipes.c

redirection.o: redirection.c
	$(CC) $(CFLAGS) -c redirection.c

background.o: background.c
	$(CC) $(CFLAGS) -c background.c

# To start over from scratch, type 'make clean': removes executable file, as well as old .o object files and *~ backup files:
clean: 
	$(RM) tups *.o *~