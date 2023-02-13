#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char** argv) {

    if(argc != 4) {
        fprintf(stderr, "Error: debes escribir ./minero <TARGET> <ROUNDS> <THREADS> para iniciar el programa.\n");
        return EXIT_FAILURE;
    }

    /* Parse the input values */
    
    printf("EL proceso principal lanza minero...\n");
    /* Lanzamos al minero*/
    execvp("./minero", argv);

    return EXIT_SUCCESS;
}