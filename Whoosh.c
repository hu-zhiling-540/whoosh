#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int const MAXARGS = 4;
int const MAXLINE = 128; // maximum length of a line of input to the shell is 128 bytes
int const MAXPATH = 100; // random number, can be disscussed later

// by line, I mean each command
int parseLine(char *cmdline, char **argv) {
  int count = 0;
  char *separator = " \n\t"; // includes space, Enter, Tab
  argv[count] = strtok(cmdline, separator); // searches for the characters listed in separator

  while ((argv[count] != NULL) && (count+1 < MAXARGS)) {
    argv[++count] = strtok((char *) 0, separator);
  }

  return count; // count of how many arguments in one command line

}

// given method for reporting error
void reportError() {
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

int whoosh_cd(char *path){
  if(path == NULL)  // when user enters cd without arguments
    return chdir(getenv("HOME"));  // change the working directory to the path stored in HOME variable
  else  // with arguments
    return chdir(path);
    // don't know if we should check if it is a vlid path
}

void whoosh_pwd(){
  char *cwd = malloc(sizeof(char *) * (MAXPATH + 1)); // still not sure if we should put extra 1 to indicate null terminator
  cwd = getcwd(cwd, (MAXPATH + 1);
  if (cwd == NULL)
    reportError();
  else
    printf("%s\n", cwd);
  free(cwd);
}

// takes nothing, so we can simply call ./woosh
int main(void){

  // int path;

  char *buffer = malloc(sizeof(char) * (MAXLINE + 1)); // +1 takes null terminator into account; not sure

  // simply check if memory allocation is successful
  if (!buffer) {
    fprintf(stderr, "allocation error\n");
    exit(EXIT_FAILURE);
  }

  // loop forever to wait and process commands
  while (1){
    printf("whoosh> "); // print shell name
    fgets(buffer, MAXLINE, stdin); // reads in text; stdin is standard input

    if (buffer == NULL) {// if user invokes EOF in stdin{
      reportError();
      continue;
    }

    char** line_argv = malloc(sizeof(char*) * MAXARGS);
    // call parseline to build argc/argv *
    int line_argc = parseLine(buffer, line_argv); // how many arguments a command takes

    // check
    printf("number of arguments for the cmd %d \n", line_argc);
    for (int i = 0; i < line_argc; i++)
      printf("print argv #%d: %s \n", i, line_argv[i]);

    if (line_argc > MAXLINE){
      reportError();
      continue;
    }

    if (strcmp(line_argv[0], "cd") == 0)
      whoosh_cd(line_argv[1]);

    if (strcmp(line_argv[0], "pwd") == 0)
      whoosh_pwd(line_argv[1]);

    if (strcmp(line_argv[0], "exit") == 0)  {
      free(line_argv);
      free(buffer);
      exit(0);
    }

    // If user hits enter key without a command
    // continue to loop again at the beginning
    // if (line_argc == 0){
    //   // char* path = NULL;
    //   continue;
    // }

    free(line_argv);
  }
  free(buffer);
  return 0;
}
