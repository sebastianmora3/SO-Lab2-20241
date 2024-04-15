#include <stdio.h>
#include <stdlib.h>
#include <string.h>	//strcmp
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_LINE_LENGTH 1024

void remover_salto_de_linea(char *str);
char** parse_line(char *line);
void mensaje_de_error();
void execute_command(char **args);
void modo_batch(int argc, char *argv[]);
char **args;

int main(int argc, char *argv[]) {

	char *line = NULL;
    size_t len = 0;
    ssize_t read;

	modo_batch(argc, argv);

	printf("wish> ");
	read = getline(&line, &len, stdin);
	remover_salto_de_linea(line);

	while(1){
		if (strcmp(line, "exit") == 0)
		{
			free(line);
			exit(0);
		}
		else if (read != -1) {
        	args = parse_line(line);	//ejecutar comando
        	execute_command(args);
    	} else {
        	void mensaje_de_error();
        	break;
    	}
    	printf("wish> ");
		read = getline(&line, &len, stdin);

		remover_salto_de_linea(line);
	}
    free(line);
    return(0);
}

const char *mypath[] = {
  "./",
  "/usr/bin/",
  "/bin/",
  NULL
};

void remover_salto_de_linea(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

char** parse_line(char *line) {
    int bufsize = MAX_INPUT_SIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        void mensaje_de_error();
        exit(1);
    }

    token = strtok(line, " \t\n");
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += MAX_INPUT_SIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                void mensaje_de_error();
                exit(1);
            }
        }

        token = strtok(NULL, " \t\n");
    }
    tokens[position] = NULL;
    return tokens;
}

void mensaje_de_error(){
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}

void execute_command(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Proceso hijo
        char* exec_path = NULL;
        for (int i = 0; mypath[i] != NULL; i++) {
            // Construir la ruta completa al ejecutable
            int path_len = strlen(mypath[i]) + strlen(args[0]) + 2; // +2 para el '/' y el NULL terminator
            exec_path = malloc(path_len);
            if (exec_path == NULL) {
                void mensaje_de_error();
                exit(1);
            }
            snprintf(exec_path, path_len, "%s/%s", mypath[i], args[0]);
            // Intentar ejecutar el comando en el directorio actual de mypath
            execvp(exec_path, args);
            // Liberar la memoria utilizada por exec_path
            free(exec_path);
        }
        // Si llegamos aquí, Comando no encontrado
        void mensaje_de_error();
        exit(1);
    } else if (pid < 0) {
        // Error al crear el proceso hijo
        void mensaje_de_error();
    } else {
        // Proceso padre
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

void modo_batch(int argc, char *argv[]){
		if(argc > 2){
			mensaje_de_error();
		}else if(argc == 2){
			FILE *archivo = fopen(argv[1], "r");
    		if (archivo == NULL) {
        		fprintf(stderr, "No se pudo abrir el archivo '%s'\n", argv[1]);
        		exit(1);
    		}
    		char linea[MAX_LINE_LENGTH];
    		while (fgets(linea, MAX_LINE_LENGTH, archivo) != NULL) {
    			args = parse_line(linea);	//ejecutar comando
        		execute_command(args);
    		}
    		fclose(archivo);
    		exit(0);
		}
	}






