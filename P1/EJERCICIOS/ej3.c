/**
 * @file ej3.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 * Escribir un programa que abra un fichero indicado por el primer parametro en modo lectura
 * usando la funcion fopen. En caso de error de apertura, el programa mostrara el mensaje de
 * error correspondiente por pantalla usando perror
 * a) ¿Qué mensaje se imprime al intentar abrir un fichero inexistente? ¿A qué valor de errno corresponde?
 * 
 *      No such file or directory
        Valor de errno: 2
 * 
 * b) ¿Qué mensaje se imprime al intentar abrir el fichero /etc/shadow? ¿A qué valor de errno corresponde?
 * 
 *      Permission denied
        Valor de errno: 13

 * c) Si se desea imprimir el valor de errno antes de la llamada a perror, ¿qué modificaciones se deberían
 * realizar para garantizar que el mensaje de perror se corresponde con el error de fopen?
 * 
 *      printf("Valor de errno: %d\n", errno);
        perror("");
 * 
 */

#include <stdio.h>
#include <errno.h>

int main(void){

    FILE *fichero;
    fichero = fopen("/etc/shadow", "r");

    if (fichero == NULL){
        printf("Valor de errno: %d\n", errno);
        perror("");
    }

    return 0;
}