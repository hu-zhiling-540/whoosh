#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

int const MAXARGS = 10;   // unknown
int const MAXLINE = 128;  // maximum length of a line of input to the shell
int const MAXPATH = 128;  // unknown
char **PATHS;             // list of all paths
int num_of_paths = 1;     // default

/* Declariations of functions */
int parseLine(char *cmdline, char **argv);
void reportError();
int whoosh_cd(char *path);
void whoosh_pwd();
void whoosh_setpath(char **argv);
void whoosh_printpath();
char* get_file_path(char* file);
int whoosh_external_cmd(char *filepath, char** argv);

/** takes nothing, so we can simply call ./woosh */
int main(int argc, char **argv){
  // error case: an incorrect number of command line arguments is passed to your shell program.
  if(argc != 1){
    reportError();
    exit(EXIT_FAILURE); // exit gracefully
  }

  PATHS = malloc(sizeof(char*) * num_of_paths); // numbers of paths at least 1
  PATHS[0] = "/bin";  // default path

  char *buffer = malloc(sizeof(char) * (MAXLINE + 1)); // +1 takes null terminator into account; not sure

  // check if memory allocation is successful
  if (!buffer) {
    reportError();
    exit(EXIT_FAILURE);
  }

  // loop forever to wait and process commands
  while (1){
    printf("whoosh> "); // print shell name
    fgets(buffer, MAXLINE, stdin); // reads in text; stdin is standard input

    if (buffer == NULL) { // if user invokes EOF in stdin
      reportError();
      continue; // skips the rest
    }

    char** line_argv = malloc(sizeof(char*) * MAXARGS);

    // call parseline to build argc/argv *
    int line_argc = parseLine(buffer, line_argv); // how many arguments a command takes

    if (line_argv[0] == NULL){ // if its' an empty command line
      reportError();
      free(line_argv);
      continue; // skips the rest
    }
    // check what command is being called
    // printf("number of arguments for the cmd %d \n", line_argc);
    // for (int i = 0; i < line_argc; i++)
    //   printf("print argv #%d: %s \n", i, line_argv[i]);

    if (line_argc > MAXLINE){
      reportError();
      continue;
    }
    // changes the directory to the users' specified directory or to home directory
    if (strcmp(line_argv[0], "cd") == 0)
      whoosh_cd(line_argv[1]);

    // prints the current directory
    else if (strcmp(line_argv[0], "pwd") == 0)
      whoosh_pwd(line_argv[1]);

    // add a new directory to PATHS variable
    else if (strcmp(line_argv[0], "setpath") == 0)
      whoosh_setpath(&line_argv[1]);

    // prints all path variables
    else if (strcmp(line_argv[0], "printpath") == 0)
      whoosh_printpath();

    // exits the whoosh program
    else if (strcmp(line_argv[0], "exit") == 0)  {
      free(line_argv);
      free(buffer);
      for(int i = 1; i < num_of_paths; i++)
        free(PATHS[i]);
      free(PATHS);
      exit(0);
    }

    // runs external programs
    else{
      char* file_path = get_file_path(line_argv[0]);
      if(file_path == NULL){  // if file not found
        reportError();
        continue;
      }
      else  {   //If it is not one of these internal commands, check if it is an external command
        whoosh_external_cmd(file_path, line_argv);
      }
      free(file_path);
    }
    free(line_argv);
  }
  free(buffer);
  return 0;
}

/* Separates arguments entered by the user */
int parseLine(char *cmdline, char **argv) {
  int count = 0;
  char *separator = " \n\t"; // includes space, Enter, Tab
  argv[count] = strtok(cmdline, separator); // searches for the characters listed in separator

  while ((argv[count] != NULL) && (count+1 < MAXARGS))
    argv[++count] = strtok((char *) 0, separator);

  return count; // count of how many arguments in one command line
}


/* Change the directory */
int whoosh_cd(char *path){
  if(path == NULL)  // when user enters cd without arguments
    return chdir(getenv("HOME"));  // change the working directory to the path stored in HOME variable
  else  // with arguments
    return chdir(path); //change the directory to the entered path and return that path
}

/* Print current working directory */
void whoosh_pwd(){
  char *cwd = malloc(sizeof(char *) * (MAXPATH + 1)); //allocate memory for the variable
  cwd = getcwd(cwd, (MAXPATH + 1)); //Set cwd to the current working directory
  if (cwd == NULL)
    reportError(); //If there is no current working directory give error message
  else
    printf("%s\n", cwd); //Or print the working directory
  free(cwd);
}

/* Set path to inputted path */
void whoosh_setpath(char **argv){
  if(argv[0] == NULL) // if the user sets path to be empty
    return; //Don't do anything

  int count = 0;  // count of paths in user input arguments
  while(argv[count] != NULL)
    count++; //When there is more than one path, increase the path count

  PATHS = realloc(PATHS, sizeof(char *) * ( count + num_of_paths));

  int i = num_of_paths;
  // add new directories to PATHS
  for (int j = 0; j < count; j++){
    PATHS[i] = malloc(sizeof(char) * (strlen(argv[j]) + 1));
    strcpy(PATHS[i], argv[j]);
    i++;
  }
  num_of_paths = i;
  return;
}

/* Print all paths that have been set */
void whoosh_printpath() {
  for(int i = 0; i < num_of_paths; i++)
    printf("%s\n", PATHS[i]); //Go through the paths list and print each one
}

/* Return the path of a given file */
char* get_file_path(char* file) {
  int i = 0;
  int found = 0;  //not found
  struct stat fileStat;
  char* temp = malloc(sizeof(char) * (MAXLINE + 1));
  while ((PATHS[i] != NULL)){ //if the path is not null
    strcpy(temp, PATHS[i]); //contatencate the path, a "/" and the file name to make the full path.
    strcat(temp, "/");
    strcat(temp, file);
    if (stat(temp, &fileStat) == 0) { //successfully
      found = 1;
      break;
    }
    i++;
  }

  if (found != 1)  {  // file not found
    free(temp);
    return NULL;
  }

  return temp; //if the file was found, return that file
}

/* Search directories for external commands and execute them */
int whoosh_external_cmd(char *filepath, char** argv){
  pid_t child_pid;
  int child_status;

  child_pid = fork();
  if(child_pid == 0) {  // the child process
    execv(filepath, argv); //If execv returns, it must have failed
    reportError();  // when a command does not exist or cannot be executed.
  }
  else if(child_pid > 0)  // run by the parent; wait for the child to terminate
    wait(&child_status);
  else
    reportError();

  return child_status;
}

/* Given method for reporting error */
void reportError() {
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}
