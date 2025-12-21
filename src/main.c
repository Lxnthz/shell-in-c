#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

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

      // Get the PATH environment variable
      char *path_env = getenv("PATH");
      if (path_env == NULL) {
        fprintf(stderr, "type: PATH environment variable not set\n");
        continue;
      }

      // Split PATH into directories
      char *path = strdup(path_env);
      char *dir = strtok(path, ":");
      int found = 0;

      while (dir != NULL) {
        // Construct the full path to the command
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, args[1]);

        // Check if the file exists and is executable
        struct stat sb;
        if (stat(full_path, &sb) == 0 && (sb.st_mode & S_IXUSR)) {
          printf("%s is %s\n", args[1], full_path);
          found = 1;
          break;
        }

        dir = strtok(NULL, ":");
      }

      free(path);

      if (!found) {
        fprintf(stderr, "%s: not found\n", args[1]);
      }

      continue; // Skip forking and executing
    }

    // Handle external programs
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
