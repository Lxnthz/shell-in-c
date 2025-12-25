#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>

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

    // Tokenize the command with support for single/double quotes and backslash escaping
    int i = 0;
    int in_single_quotes = 0;
    int in_double_quotes = 0;
    char *current_arg = malloc(256); // Temporary buffer for the current argument
    int arg_len = 0; // Track the length of current_arg

    char *p = command;
    while (*p != '\0') {
      if (*p == '\\' && !in_single_quotes) {
        // Handle backslash escaping
        if (in_double_quotes) {
          // Inside double quotes, only escape certain characters
          p++;
          if (*p == '"' || *p == '\\') {
            current_arg[arg_len++] = *p; // Add the escaped character
          } else {
            current_arg[arg_len++] = '\\'; // Treat backslash literally
            if (*p != '\0') {
              current_arg[arg_len++] = *p; // Add the next character
            }
          }
          if (*p != '\0') p++;
        } else {
          // Outside quotes, escape the next character
          p++;
          if (*p != '\0') {
            current_arg[arg_len++] = *p;
            p++;
          }
        }
      } else if (*p == '\'' && !in_double_quotes) {
        // Toggle single-quote mode; treat contents literally
        in_single_quotes = !in_single_quotes;
        p++;
      } else if (*p == '"' && !in_single_quotes) {
        // Toggle double-quote mode
        in_double_quotes = !in_double_quotes;
        p++;
      } else if (isspace((unsigned char)*p) && !in_single_quotes && !in_double_quotes) {
        // Found a delimiter outside quotes
        if (arg_len > 0) {
          current_arg[arg_len] = '\0';
          args[i++] = strdup(current_arg);
          arg_len = 0;
        }
        while (isspace((unsigned char)*p)) p++;
      } else {
        // Regular character (inside or outside quotes)
        current_arg[arg_len++] = *p;
        p++;
      }
    }

    // Add the last argument if any
    if (arg_len > 0) {
      current_arg[arg_len] = '\0';
      args[i++] = strdup(current_arg);
    }

    args[i] = NULL;
    free(current_arg);

    // Remove quotes from the executable name (first token)
    if (args[0] != NULL) {
      char *executable = args[0];
      int exec_len = strlen(executable);
      if ((executable[0] == '\'' && executable[exec_len - 1] == '\'') ||
          (executable[0] == '"' && executable[exec_len - 1] == '"')) {
        // Remove the surrounding quotes
        executable[exec_len - 1] = '\0';
        memmove(executable, executable + 1, exec_len - 1);
      }
    }

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

    // Handle the "pwd" command
    if (strcmp(args[0], "pwd") == 0) {
      char cwd[1024];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
      } else {
        perror("pwd");
      }
      continue; // Skip forking and executing
    }

    // Handle the "cd" command
    if (strcmp(args[0], "cd") == 0) {
      if (args[1] == NULL) {
        fprintf(stderr, "cd: missing argument\n");
      } else if (strcmp(args[1], "~") == 0) {
        // Handle "cd ~"
        char *home = getenv("HOME");
        if (home == NULL) {
          fprintf(stderr, "cd: HOME environment variable not set\n");
        } else {
          if (chdir(home) != 0) {
            fprintf(stderr, "cd: %s: No such file or directory\n", home);
          }
        }
      } else {
        // Handle other paths
        if (chdir(args[1]) != 0) {
          fprintf(stderr, "cd: %s: No such file or directory\n", args[1]);
        }
      }
      continue; // Skip forking and executing
    }

    // Handle the "type" command
    if (strcmp(args[0], "type") == 0) {
      if (args[1] == NULL) {
        fprintf(stderr, "type: missing file operand\n");
        continue;
      }

      // Check if the argument is a built-in command
      if (strcmp(args[1], "echo") == 0 || strcmp(args[1], "exit") == 0 || strcmp(args[1], "type") == 0 || strcmp(args[1], "pwd") == 0 || strcmp(args[1], "cd") == 0) {
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
