#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	//recibe argumento comando	
	//valida campo no vacio
	
	assert(argc == 2 && "Cantidad de argumentos inválida");
	
	struct timeval start, end;
   	pid_t pid;
    	int status;
    	
    	gettimeofday(&start, NULL);
    	
	// Crea proceso hijo
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error al llamar a fork()\n");
        return 1;
    } else if (pid == 0) {
        // Proceso hijo ejecuta comando
        execlp(argv[1], argv[1], NULL);
        // Si execlp devuelve, hubo un error
        fprintf(stderr, "Error al ejecutar el comando\n");
        exit(1);
    } else {
        // Proceso padre espera que el proceso hijo termine
        wait(&status);
        
        gettimeofday(&end, NULL);
        
        long segundos = end.tv_sec - start.tv_sec;
        long microsegundos = end.tv_usec - start.tv_usec;
        double total = segundos + microsegundos*1e-6;
        
        printf("Tiempo transcurrido: %.6f segundos\n", total);
    }

    return 0;
	
}
