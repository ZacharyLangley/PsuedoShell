/*******************************************************
        CSCE 3600: Systems Programming
        Major Assignment #1 : Shell
        Professor: Dr. Mark A. Thompson, Sr
*********************************************************/

/*******************************************************
                Assignment Description:
  The purpose of this assignment is to create a shell that
  will run commands. The commands will either be enter by
  the user manually or via an input file. The commands are
  stored in an array, parsed, and executed. The shell is
  designed to simulate a linux shell, such as PuTTY, though
  with limited functionality. However, the shell program
  can still run commands as though the commands were being
  run from the main shell.
*******************************************************/


//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

//Structure of Arguments
struct cmd {
 char* cmd_name;      // Name of a command
 char* argument[20]; // Array of arguments of this command
 int type;
 int arg_count;      // hold a total argument of a command
 };

//Function Prototypes
void reset(struct cmd A[20], int);
void pro_cmd2(struct cmd );
void pro_input(char *);
void user_mode();
int  batch_mode(const char * batchFile);
void pro_cmd(struct cmd );
char *trim(char *str);
void parse(struct cmd A[20], int *, char * );
void parse_arg(struct cmd A[20], int);

//Global Variables
char * history[1024];
char * prompt;
int c_line = 0;


//Function Name: Main
//Function Purpose: main is standard in all programs
//-creates initial history
//-goes to either interactive mode or batch mode
int main(int argc, char ** argv){

   // validate correcct parameters
   if( argc > 2 )
   {
     printf("usage: shell [batchFile]\n");
     exit(-1);
   }

//Initial history list
c_line = 0;
int i = 0;
for (i = 0; i < 1024; i++){ history[i] = (char *) malloc(sizeof(char) * (1024 + 1)); }
 prompt = (char *) malloc(sizeof(char) * (1024 + 1));
 strcat(prompt, "prompt>");

 if (argc == 1){ user_mode(); } //user mode
 else if (argc == 2){ batch_mode(argv[1]); } //batch mode

 return 0;
} //main

//Function name: Process Input (pro_input)
//Function Purpose: Process input from file (bash mode) or user (interactive mode)
//-gets a command count
//-puts the commands into an array
//-sends the commands to be parsed and then processed
void pro_input(char * input) {
 struct cmd A[20];   // array to hold all commands from interactive or batch mode
 int cmd_count = 0, j = 0, i = 0;

 if (strcmp(input, "history") == 1 ){
  strcpy(history[c_line], input);
  c_line++;
 }//if

 // Initial list of commands
 for (i = 0; i < 20; i++){
  A[i].cmd_name = (char *) malloc(sizeof(char) * (1024 + 1));
  for (j = 0; j < 20; j++){ A[i].argument[j] = (char *) malloc(sizeof(char) * (1024 + 1)); }//for
   A[i].type = 0; // 0 == command without argument
   A[i].arg_count = 0;
  }//for

 //parse the commands line from user input
 parse(A, &cmd_count, input);

 //parse command argument from each commands.
 parse_arg(A, cmd_count);

 for (i = 0; i < cmd_count; i++){
  // process all commands here
  if (A[i].type != 2)
   printf("\n");

   if (A[i].type == 2){
    pro_cmd2(A[i]);
    continue; //continue process other command if any
   }//if
   pro_cmd(A[i]);
  }//if

  //reset command struct, ready for next input
  reset(A, cmd_count);
  cmd_count = 0;
 }//function

//Function Name: Process Command (pro_cmd)
//Function Purpose:  This function will run each command in a child process
//-forks the child process
//-runs execvp for the command
//-checks for errors running execvp
void pro_cmd(struct cmd A) {
 while(1){
  if(fork() == 0){ //child
   if (A.type == 0) { // w/o arg
    execlp(A.cmd_name, A.cmd_name, (char *)0 );
    perror ("The following error occurred:\n");
   }//if
   else if (A.type == 1) { //has arg
    char *arg[10];
    arg[0] = A.cmd_name;
    int i;
    for (i = 0; i < A.arg_count; i++) { arg[i + 1] = A.argument[i]; }//for
    arg[i + 1] = NULL;
    execvp(arg[0], arg);
    perror ("The following error occurred:\n");
   }//else if
   exit(1);
  }//if
  else { wait( (int *)0 ); break; } //else
 }//while
}//function

//Function Name: Trim "space" characters
//Function Purpose: Trim off any extra white space in a command call
// - This function returns a pointer to a substring of the original string.

char *trim(char *string) {
 char *tail;

 // Trim leading space
 while(isspace(*string)) { string++; } //while
 if(*string == 0) { return string; } //if

 // Trim trailing space
 tail = string + strlen(string) - 1;
 while(tail > string && isspace(*tail)) { tail--; } //while

 // Write new null terminator
 *(tail+1) = 0;

 return string;
}

//Function Name: Parse
//Function Purpose: This function will parse all commands from a string, then save them to an array of all commands
//-seperates commands by ;
//-checks if the command is a special command
void parse(struct cmd A[20], int * cmd_count, char * input ) {
 char * token;
 token = strtok (input, ";"); /* each command seperated by semicolon ; character */
 //seperate command from user input
 if (token != NULL){
  A[(*cmd_count)].cmd_name = trim(token);
  // handle special command here
  if (strcmp(A[(*cmd_count)].cmd_name, "quit") == 0 || strcmp(A[(*cmd_count)].cmd_name, "exit") == 0 || strcmp(A[(*cmd_count)].cmd_name, "cd") == 0 || strcmp(A[(*cmd_count)].cmd_name, "history") == 0 || strcmp(A[(*cmd_count)].cmd_name, "prompt") == 0) {
   A[(*cmd_count)].type = 2; //special command
  }//if

  (*cmd_count) ++;
 }//if
 do{
  token = strtok (NULL, ";");			// the NULL here means use the same line of characters we started with
  if (token == NULL) { continue; }
  A[(*cmd_count)].cmd_name = trim(token);
  (*cmd_count)++;
 } while (token != NULL);
}//function

//Function Name: Parse Argument (parse_arg)
//Function Purpose: This function will parse all argument in each command, then update its command name if any
//-parses each argument by " "
//-sends each argument to be trimmed
//-checks if each argument is a special command
void parse_arg(struct cmd A[20], int cmd_count) {
 int i;
 for (i = 0; i < cmd_count; i++){
  char * command = A[i].cmd_name;
  int j = 0; /* number of argument of each command */
  char * token;
  token = strtok (command, " "); /* each argumenr seperated by space " " character */
  do{
   token = strtok (NULL, " ");	// the NULL here means use the same line of characters we started with
   if (token == NULL) { continue; }//if
   A[i].argument[j] = trim(token);
   j++;
  } while (token != NULL);
  if (j > 0) { // update
   if (A[i].type == 0) { A[i].type = 1; }
   if (strcmp(A[i].cmd_name, "cd") == 0 || strcmp(A[i].cmd_name, "history") == 0 || strcmp(A[i].cmd_name, "prompt") == 0) {
     A[i].type = 2;
   }//if
   A[i].arg_count = j; /* update total argument */
  }//if
 }//for
}//function

//Function Name: Process Command Type 2 (pro_cmd2)
//Function Purpose: process the special commands
//-if command is quit/exit, exits the shell
//-if command is cd, changes directory
//-if command is history, prints history
//-if command is prompt, prompts for a prompt command
void pro_cmd2(struct cmd A) {
 if (strcmp(A.cmd_name, "quit") == 0 || strcmp(A.cmd_name, "exit") == 0) { 
   exit(0); 
 } // if
 if (strcmp(A.cmd_name, "cd") == 0) {
  //process cd arguments command.	
  int ret;
  if (A.arg_count > 0) {
   ret = chdir (A.argument[0]);
   getcwd(A.argument[0],sizeof(A.argument[0]));
  }//if (arg_count > 0 )
  else if (A.arg_count == 0) {
   ret = chdir (" ");
   getcwd(" ", 1);
  }//else if (arg count == 0)
  if(ret!=0) { perror("Error while process the cd command: "); } //if
 }//if (cmd_name == "cd" )

  // history & prompt command
  if (strcmp(A.cmd_name, "history") == 0) { //history cmd
   int i;
   for (i = 0; i < c_line; i++){ printf("%s\n", history[i]); }
  }// if (cmd_name == "history")
  else if (strcmp(A.cmd_name, "prompt") == 0) {
   //process prompt command.
   if (A.arg_count > 0) { strcpy(prompt, A.argument[0]); } //if
   else if (A.arg_count == 0) { printf("Please enter argument for prompt command \n"); }//else if ( arg_count == 0)
  } // else if (cmd_name == "prompt")

}//function

//Function Name: Reset
//Funtion Purpose: resets the shell after running a command
//-clears out the array
void reset(struct cmd A[20], int cmd_count){
 int i = 0, j = 0;
 for (i = 0; i < cmd_count; i++) {
  memset(A[i].cmd_name, '\0', 1024);
  for (j = 0; j < A[i].arg_count; j++) { memset(A[i].argument[j], '\0', 1024); }//for
  A[i].type = 0;
  A[i].arg_count = 0;
 }//for
}//function

//Function Name: User Mode
//Function Purpose: Handle shell in interactive mode
//-gives the user the prompt with instructions
//-checks for ctrl+D, exits if so
//-sends commands to be processed after removing \n
void user_mode(){
// interactive shell mode
 printf("***interactive mode***\n");
 //bonus ***
 printf("--> type ""prompt"" at any time to change prompt\n");
 printf("--> type ""history"" at any time to display history\n");

 while (1){
 //interactive mode
 printf("%s ", prompt);

 //read command
 char * Input = (char*)malloc(1024);		

 //read command from prompt
 fgets (Input, 1024, stdin);

 //process for CTRL-D
 if (Input == NULL){
 char c;
 printf("Write: ");
 scanf("%c", &c);
 if(c == 0x04){
 printf("ctrl+D entered\n");
 return;
 }//if

}//while

 //Remove trailing newline, if there.
 if ((strlen(Input)>0) && (Input[strlen (Input) - 1] == '\n'))
  Input[strlen (Input) - 1] = '\0';
 // Process input from user
 pro_input(Input);						
 }//if
}//function

//Function Name: Batch Mode
//Function Purpose: Handle shell in Batch Mode
//-opens up the input file, closes if no file
//-gets the commands from the file, one line at a time
//-sends commands to be processed
int batch_mode(const char * batchFile) {
//read command
 char * line = (char *) malloc(1024);

 // open and get the file handle
 FILE* fp;
 fp = fopen(batchFile, "r");
 //check if file exists
 if (fp == NULL){
  printf("ERROR: File %s not found\n", batchFile);
  return 0;
 }//if
 printf("*** batch mode ***\n");
 printf("--> file: %s\n", batchFile);
 //read line by line
 while (1) {
  if ( fgets(line, 1024 , fp) == NULL) break;
  // Remove trailing newline, if there.
  if ((strlen(line) > 0) && (line[strlen (line) - 1] == '\n'))
   line[strlen (line) - 1] = '\0';
  //Print command line from bash file
  printf("batch line>  %s\n", line);
  // Process input from bash file
  pro_input(line);
 }//while
 return 0;
}//function
