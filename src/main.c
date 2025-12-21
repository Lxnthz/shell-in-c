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

    if (strcmp(command, "exit") == 0) {
      if (args[1] != NULL) {
        int exit_code = atoi(args[1]);
        exit(exit_code);
      } else {
        exit(0);
      }
    }

    int i = 0;
    char *token = strtok(command, " ");
    while (token != NULL) {
      args[i++] = token;
      token = strtok(NULL, " ");
    }
    args[i] = NULL;

    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      continue;
    }

    if (pid == 0) {
      if (execvp(args[0], args) == -1) {
        perror(args[0]);
      }
      exit(EXIT_FAILURE);
    } else {
      int status;
      waitpid(pid, &status, 0);
    }

    printf("%s: command not found\n", command);
  }

  return 0;
}
