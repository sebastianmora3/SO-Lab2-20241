#include <pthread.h> // pthread_create, pthread_join
#include <regex.h>   // regcomp, regexec, regfree
#include <stdio.h>   // fopen, fclose, fileno, getline, feof
#include <stdlib.h>  // exit
#include <string.h>  // strlen, strsep, strcat, strdup, strcmp
#include <unistd.h>  // STDERR_FILENO, fork, exec, access, exit, chdir
#include <sys/wait.h> // waitpid
#include <sys/types.h> // pid_t

// Function prototypes
void execute_command(char *command, char *args[]);
void handle_builtin_command(char *command, char *args[]);
void execute_line(char *line);

void printError() {
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

#define MAX_ARGS 64
#define MAX_PATHS 64
#define MAX_LINE_LENGTH 1024

char *builtin_commands[] = {"exit", "cd", "path"};
char *paths[MAX_PATHS];
int num_paths = 0;

void execute_command(char *command, char *args[]) {
    pid_t pid = fork();
    if (pid < 0) {
        printError();
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        execvp(command, args);
        // If execvp returns, it must have failed
        printError();
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}

void handle_builtin_command(char *command, char *args[]) {
    if (strcmp(command, "exit") == 0) {
        exit(EXIT_SUCCESS);
    } else if (strcmp(command, "cd") == 0) {
        if (args[1] == NULL) {
            printError();
            fprintf(stderr, "cd: No such file or directory\n");
        } else if (chdir(args[1]) != 0) {
            printError();
        }
    } else if (strcmp(command, "path") == 0) {
        num_paths = 0;
        for (int i = 1; args[i] != NULL; i++) {
            paths[num_paths++] = args[i];
        }
        paths[num_paths] = NULL;
    }
}

void execute_line(char *line) {
    char *args[MAX_ARGS];
    char *token;

    // Parse the command line
    int i = 0;
    token = strtok(line, " \t\n");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;

    if (i == 0) {
        return; // Empty command line
    }

    // Check if it's a built-in command
    for (int j = 0; j < sizeof(builtin_commands) / sizeof(char *); j++) {
        if (strcmp(args[0], builtin_commands[j]) == 0) {
            handle_builtin_command(args[0], args);
            return;
        }
    }

    // If not a built-in command, check if executable in path
    for (int j = 0; j < num_paths; j++) {
        char path[strlen(paths[j]) + strlen(args[0]) + 2]; // +2 for / and null terminator
        strcpy(path, paths[j]);
        strcat(path, "/");
        strcat(path, args[0]);
        if (access(path, X_OK) == 0) {
            execute_command(path, args);
            return;
        }
    }

    fprintf(stderr, "%s: command not found\n", args[0]);
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        printError();
        exit(EXIT_FAILURE);
    }

    FILE *input_file = stdin;
    if (argc == 2) {
        input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
            printError();
            exit(EXIT_FAILURE);
        }
    }

    // Initialize paths with default /bin
    paths[0] = "/bin";
    paths[1] = NULL;
    num_paths = 1;

    char *line = NULL;
    size_t line_size = 0;
    ssize_t read;

    if (argc == 1) {
        while (1) {
            printf("wish> ");
            fflush(stdout); // Asegura que el prompt se imprima antes de leer la entrada
            if ((read = getline(&line, &line_size, input_file)) == -1) {
                break; // Sal del bucle si hay un error o se alcanza el final de archivo
            }
            execute_line(line);
        }
    }

    free(line);
    if (input_file != stdin) {
        fclose(input_file);
    }

    return EXIT_SUCCESS;
}
