#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  char command[256];
  char *args[10];

  while (1) {
    printf("$ ");

    // Read user input
    if (fgets(command, sizeof(command), stdin) == NULL) {
      break;
    }

    // Remove trailing newline character
    command[strcspn(command, "\n")] = '\0';

    // Tokenize the command
    int i = 0;
    char *token = strtok(command, " ");
    while (token != NULL) {
      args[i++] = token;
      token = strtok(NULL, " ");
    }
    args[i] = NULL;

    // Handle the "exit" command
    if (strcmp(args[0], "exit") == 0) {
      if (args[1] != NULL) {
        int exit_code = atoi(args[1]);
        exit(exit_code);
      } else {
        exit(0);
      }
    }

    // Handle the "echo" command
    if (strcmp(args[0], "echo") == 0) {
      for (int j = 1; args[j] != NULL; j++) {
        printf("%s", args[j]);
        if (args[j + 1] != NULL) {
          printf(" "); // Add a space between arguments
        }
      }
      printf("\n");
      continue; // Skip forking and executing
    }

    // Handle the "type" command
    if (strcmp(args[0], "type") == 0) {
      if (args[1] == NULL) {
        fprintf(stderr, "type: missing file operand\n");
        continue;
      }

      // Check if the argument is a built-in command
      if (strcmp(args[1], "echo") == 0 || strcmp(args[1], "exit") == 0 || strcmp(args[1], "type") == 0) {
        printf("%s is a shell builtin\n", args[1]);
        continue;
      }

      // Otherwise, check if it's an external file
      FILE *file = fopen(args[1], "r");
      if (file == NULL) {
        fprintf(stderr, "%s: not found\n", args[1]);
        continue;
      }

      fclose(file);
      printf("%s is an external file\n", args[1]);
      continue; // Skip forking and executing
    }

    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      continue;
    }

    if (pid == 0) {
      // Child process: execute the command
      if (execvp(args[0], args) == -1) {
        // Print the expected error message format
        fprintf(stderr, "%s: command not found\n", args[0]);
      }
      exit(EXIT_FAILURE);
    } else {
      // Parent process: wait for the child to finish
      int status;
      waitpid(pid, &status, 0);
    }
  }

  return 0;
}
