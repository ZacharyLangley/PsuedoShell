I. Purpose
-----------------------------------------------------------------------
 The purpose of the assignment was to challenge the students to create 
 a shell program. The shell program will run commands in one of two  
 ways. If the user just runs the executable, the shell will run in 
 interactive mode. If an input file is provided, the shell will run in 
 batch mode. In interactive mode, the user is prompted to manually 
 enter commands. The program will execute the commands and the user  
 will be prompted again. The program will continue to run until the 
 user enters “quit.” In batch mode, the program will execute the 
 commands in the input file one line at a time. After the commands 
 have run, the program ends.

II. File List
-----------------------------------------------------------------------
- shell.c
- batch1
- batch2
- README.md
- Makefile

III. Compiling the assignment
-----------------------------------------------------------------------
 Simply upload all the file to your directory and run "make" to 
 compile all the included files,in this case, we only have one
 .c file. (no separate header files or .c are included)

VI. Instructions
-----------------------------------------------------------------------
1. Unzip files
2. Enter "make" into command prompt
3. Type "./shell" into command prompt
4. Enter in Commands
5. Seperate commands with "; "


V. Design Overview
-----------------------------------------------------------------------
 The program is set up using a struct array to hold the commands. Each 
 struct has a command name, a command type, the command(s), and the 
 argument count. During program startup, the program checks for the 
 initial number of arguments. If the argument count is 1, then the 
 program will run in interactive mode. If the argument count is 2, 
 then the program will run in batch mode. The program will then get 
 setup to run in either mode, but the shell execution runs the same 
 either way once the commands are pulled.
 
 The program then inputs the commands into the struct array. The  
 commands are then parsed. Each command gets any extra characters and 
 spaces trimmed. During parsing, the program also checks for special 
 commands, such as exit, history, or prompt. To execute the commands, 
 the command is run through a child process via execvp. If the command 
 is a special command, then the command is sent to a function to be 
 executed manually. After the commands run, the shell resets the array 
 and prepares to run the next command. In interactive mode, the user 
 is prompted again, and in bash mode, the shell pulls the next 
 commands. This repeats until all the commands are ran or the 
 interactive mode ends.

 For ambiguities, the code has a few methods for dealing with them. 
 For extra semicolons, the program reads commands until it finds a 
 semicolon, so having a few semicolons next to each other will just be 
 read over. Extra white spaces will be trimmed off in the trim 
 function. No commands will just be passed over so those are not an 
 issue.
