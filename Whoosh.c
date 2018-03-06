#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int const MAXARGS = 4;
int const MAXLINE = 128; // maximum length of a line of input to the shell is 128 bytes
int const MAXPATH = 100;

int parseline(char *cmdline, char **argv) {
  int count = 0;
  char *separator = " \n\t"; /* Includes space, Enter, Tab */
  argv[count] = strtok(cmdline, separator); // searches for the characters listed in separator

  while ((argv[count] != NULL) && (count+1 < MAXARGS)) {
    argv[++count] = strtok((char *) 0, separator);
  }

  return count;

}

void reportError() {
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

int main(void){

  // int path;

  char *buffer = malloc(sizeof(char) * MAXLINE);

  if (!buffer) {
    fprintf(stderr, "allocation error\n");
    exit(EXIT_FAILURE);
  }

  // loop forever to wait and process commands
  while (1){
    printf("whoosh> "); // print shell name
    fgets(buffer, MAXLINE, stdin); // reads in text; stdin is standard input

    char** line_argv = malloc(sizeof(char*) * 128);
    // call parseline to build argc/argv *
    int line_argc = parseline(buffer, line_argv); // how many arguments a command takes

    // check
    printf("number of arguments taken in %d \n", line_argc);

    // buffer = strtok(buffer, "\n");
    // int currLen = strlen(buffer);
    if (line_argc > MAXLINE){
      reportError();
      continue;
    }

    if (strcmp(line_argv[0], "exit") == 0)
      exit(0);
    // If user hits enter key without a command
    // continue to loop again at the beginning
    // if (line_argc != 0){
    //   // char* path = NULL;
    //   continue;
    // }
  }


}
