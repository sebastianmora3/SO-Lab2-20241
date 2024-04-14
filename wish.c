#include <stdio.h>
#include <stdlib.h>
#include <string.h>	//strcmp

void remover_salto_de_linea(char *str);

int main(int argc, char *argv[]) {
	
 	char *line = NULL;
    size_t len = 0;
    ssize_t read;

	printf("wish> ");
	read = getline(&line, &len, stdin);

	remover_salto_de_linea(line);

	while(strcmp(line, "exit") != 0){
		
		if (read != -1) {
        	printf("La línea que ingresaste es: %s\n", line);	//ejecutar comando
    	} else {
        	printf("Error al leer la línea."); // ERROR PERSONALIZADO
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









