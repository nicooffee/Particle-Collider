/**
 * Módulo participant_list:
 * 
 * Dentro de este módulo se construye el dato para simular el listado
 * de participantes de la simulación, como también las funciones para manejarlo.
 * Dentro de este módulo se mueven los participantes, tomando en cuenta las
 * colisiones con otros participantes. Este módulo es independiente de el espacio
 * donde se utilizará el listado de participantes. Se utiliza puntero opaco para 
 * abstraer a los módulos externos sobre el funcionamiento de la lista.
 */
#include <stdlib.h>
#include "participant_list.h"
#include "participant.h"





/**
 * struct Participant_list:
 * 
 * Dato para almacenar un listado de participantes y su cantidad.
 * 
 *  list            : Puntero a Participant. Se utiliza para generar un arreglo.
 *  numParticipant  : Cantidad de participantes del arreglo.
 *  numActive       : Cantidad de participantes con al menos una partícula.
 */
struct Participant_list{
    Participant *list;
    int numParticipant;
    int numActive;
};





/**
 * participant_list_create:
 * 
 * Función que genera un listado de participantes según los datos enviados por
 * parámetro. Si la cantidad de participantes es mayor a la cantidad de espacios
 * en los bordes[2*((maxx-minx)+(maxy-miny))-4], se limitará a 2*((maxx-minx)+(maxy-miny))-4.
 * In:  n           : Cantidad de participantes a generar.
 *      numParticle : Numero de partículas de cada participante.
 *      maxx,maxy   : Límites superiores para el eje X e Y de cada participante.
 *      minx,miny   : Límites inferiores para el eje X e Y de cada participante.
 * 
 * Ou:  new         : Listado con los participantes en su posición inicial.
 */
Participant_list participant_list_create(int n,int numParticle, int maxx,int maxy, int minx, int miny){
    register int i=0;
    struct Participant_list *new;
    new = (struct Participant_list *) calloc(1,sizeof(struct Participant_list));
    new->list = (Participant *) calloc(n,sizeof(Participant));
    new->numParticipant = n<2*((maxx-minx)+(maxy-miny))-4?n:2*((maxx-minx)+(maxy-miny))-4;
    new->numActive = new->numParticipant;
    for(;i<n;i++){
        new->list[i] = participant_create(numParticle,i,maxx,maxy,minx,miny);
        while(participant_list_collision(new,new->list[i])!=NULL)
            participant_reset_position(new->list[i]);
    }
    return new;
}





/**
 * participant_list_move:
 * 
 * Función que mueve un participante según una id dada. Comprueba si existen
 * colisiones y llama a participant_particle_collided si existe tal situación.
 * Si existe colisión, se asigna en coll el puntero a tal participante.
 * 
 * In:  listp   : Listado de participantes.
 *      id      : ID del participante a mover.
 *      coll    : Puntero a Participant.
 * 
 * Ou:  valores : 0 -> no hubo movimiento para id.
 *                1 -> Se movió el participante.
 *      
 *      coll    : NULL -> No hubo colisión
 *                Participante con el que colisionó p luego de moverse.
 */
int participant_list_move(Participant_list listp,Participant p,Participant *coll){ 
    int pp,pc;
    (*coll) = NULL;
    if(participant_move(p)){
        (*coll) = participant_list_collision(listp,p);
        if((*coll)==NULL) return 1;
        pp = (participant_particle_collided(p), participant_get_particleNum(p));
        pc = (participant_particle_collided((*coll)), participant_get_particleNum((*coll)));
        listp->numActive -= ((pp<=0?1:0) + (pc<=0?1:0));
        return 1;
    }
    return 0;
}





/**
 * participant_list_collision:
 * 
 * Función que comprueba si el participante p esta en colisión con otro
 * participante.
 * 
 * In:  listp   : Listado de participantes.
 *      p       : Participante a comprobar.
 *
 * Ou: valores  : Participante con el que hace colisión.
 *                NULL si no hace colisión.  
 */
Participant participant_list_collision(Participant_list listp,Participant p){
    register int i = 0;
    int posX = participant_get_x(p);
    int posY = participant_get_y(p);
    int id = participant_get_id(p);
    for(;i<participant_list_get_length(listp);i++)
        if( participant_get_x(listp->list[i])==posX && \
            participant_get_y(listp->list[i])==posY && \
            participant_get_id(listp->list[i])!=id  && \
            participant_get_particleNum(listp->list[i])!=0)
            return listp->list[i];
    return NULL;
} 





/**
 * participant_list_get_length:
 * 
 * Función que entrega la cantidad de participantes dentro de una lista.
 * 
 * In:  listp   : Listado de participantes.
 * 
 * Ou:  Cantidad de participantes.
 */
int participant_list_get_length(Participant_list listp){
    return listp->numParticipant;
}





/**
 * participant_list_get:
 * 
 * Función que entrega al participante con id entregado por parámetro.
 * 
 * In:  listp   : Listado de participantes.
 *      id      : id a buscar.
 * 
 * Ou:  Participante correspondiente al id.
 *      NULL si no existe tal participante.
 */
Participant participant_list_get(Participant_list listp,int id){
    return (id<0 || id>=listp->numParticipant)?NULL:listp->list[id];
}





/**
 * participant_list_get_numActive:
 * 
 * Función que entrega la cantidad de participantes con partículas.
 * 
 * In:  listp   : Listado de participantes.
 * 
 * Out: Cantidad de participantes activos.
 */
int participant_list_get_numActive(Participant_list listp){
    return listp==NULL?0:listp->numActive;
}





/**
 * participant_list_get_arrActive:
 * 
 * Función que recibe un doble puntero a int, cuenta los participantes
 * por id%4 y los guarda en tal puntero.
 * 
 * La función asume que el doble puntero es NULl o un arreglo de largo
 * 4.
 */
void participant_list_get_arrActive(Participant_list listp,int **c){
    register int i;
    if((*c)==NULL) (*c) = (int *) malloc(4*sizeof(int));
    for(i=0;i<4;i++) (*c)[i] = 0;
    for(i = 0;i<listp->numParticipant;i++)
        if(participant_get_particleNum(listp->list[i])!=0)
            (*c)[participant_get_id(listp->list[i])%4]++;
    return;
}



/**
 * participant_list_free:
 * 
 * Función que libera la memoria utilizada por un listp. Libera la memoria
 * de cada participante almacenado.
 * 
 * In:  listp   : Listado de participantes.
 */
void participant_list_free(Participant_list listp){
    int i = 0;
    for(;i<listp->numParticipant;i++)
        participant_free(listp->list[i]);
    free(listp->list);
    free(listp);
}