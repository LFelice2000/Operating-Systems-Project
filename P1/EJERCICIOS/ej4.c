/**
 * @file ej4.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 * EJERCICIO 4: Espera activa e inactiva
 * a) Escribir un programa que realice una espera de 10 segundos usando la función clock 
 * en un bucle. Ejecutar en otra terminal el comando top. ¿Qué se observa?
 * 
 *     Se observa que el programa se ejecuta en un bucle infinito, y que el uso de CPU es del 100%.
 * 
 * b) Reescribir el programa usando sleep y volver a ejecutar top. ¿Ha cambiado algo?
 * 
 *     Sí, ahora el uso de CPU es del 0%. 
 * 
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(void){

    clock_t start = clock();
    clock_t end = start + 10 * CLOCKS_PER_SEC;

    while (/*clock() < end*/ sleep(10));

    return 0;
}