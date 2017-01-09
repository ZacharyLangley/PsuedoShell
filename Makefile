################################################################
#
# shell.c is the name of your source code; you may change this.
# However, you must keep the name of the executable as "shell".
#
# Type "make" or "make shell" to compile your code
#
# Type "make clean" to remove the executable (and object files)
#
################################################################
CC=gcc
#CFLAGS=-Wall -g
shell: shell.c
	$(CC) -o shell shell.c
clean:
	$(RM) shell


