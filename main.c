/*--------------------------------------------------------------------------------------------------------------------**/
/************************************************************************************************************************
    Autores        : NICOLAS HONORATO DROGUETT, OSCAR FRITIS LOBOS.
	Proposito      : Laboratorio 1 algoritmos distribuidos.
    Programa       : Movimiento de 4 particulas en forma concurrente.
    Fecha          : Santiago de Chile, 24 de mayo de 2019
	Compilador C   : gcc (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0
************************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>


/**
 * Parametros de inicio:
 *  - n:    Cantidad de particulas de cada participante.
 *  - k:    Cantidad de participantes.
 *  - i:    Iteraciones del proceso.
 *  - d:    Delay de refresco de pantalla.
 */
int main(int args, char **argv){
    int n,k,i,d;

    if(args != 5){
        printf("Parametros de ejecucion incorrectos.");
        exit(0);
    }
    n = atoi(argv[1]);
    k = atoi(argv[2]);
    i = atoi(argv[3]);
    d = atoi(argv[4]);
}