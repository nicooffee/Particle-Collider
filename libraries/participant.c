
/**
 * Módulo participant.
 * 
 * Dentro de este módulo se construye el dato para simular un participante
 * singular dentro de la simulación, como también las funciones para editar sus datos.
 * El dato Participant es independiente de la existencia de otros Participant dentro
 * de la misma posición. Se utiliza un puntero opaco para abstraer a los módulos externos
 * sobre el comportamiento del dato Participant.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "participant.h"

/**
 * Struct participant:
 * Estructura utilizada para almacenar a los participantes de la simulación.
 * 
 *  id            : Numero del participante.
 *  particleNum   : Numero de particulas actual.
 *  posX, posY    : Posición actual.
 *  minX, minY    : Limites inferiores para posX y posY
 *  maxX, maxY    : Limites superiores para posX y posY
 *  dirX, DirY    : Direcciones de movimiento para eje X e Y.
 */
struct Participant{
    int id;
    int particleNum;
    int posX;
    int posY;
    int minX;
    int minY;
    int maxX;
    int maxY;
    int dirX;
    int dirY;
};





/**
 * participant_create:
 * 
 * Función que genera un participante de acuerdo a los datos recibidos. Se le
 * entrega una posición según la función participant_reset_position.
 * 
 * In:  parNum      : Cantidad de particulas.
 *      participant : ID del participante.
 *      maxx,maxy   : Limite superior del eje X e Y.
 *      minx,miny   : Limite inferior del eje X e Y.
 * 
 * Out: new         : Participante generado.
 */
Participant participant_create(int parNum, int participant , int maxx, int maxy, int minx, int miny){
    struct Participant *new;
    new = (struct Participant*) calloc(1,sizeof(struct Participant));
    new->id = participant;
    new->particleNum = parNum;
    new->minX = minx;
    new->minY = miny;
    new->maxX = maxx;
    new->maxY = maxy;
    participant_reset_position(new);
    return new;
}





/**
 * participant_reset_position:
 * 
 * Función que genera una posición inicial para un participante. Los criterios
 * para la posición son los siguientes:
 *  Si ID mod 4 = 0 -> lado izquierdo.
 *  Si ID mod 4 = 1 -> lado inferior.
 *  Si ID mod 4 = 2 -> lado derecho.
 *  Si ID mod 4 = 3 -> lado superior.
 * 
 * In:  p: Participante al cual se le genera la posición.
 */
void participant_reset_position(Participant p){
    int pos;
    int maxx,maxy;
    if (p==NULL || p->particleNum==0) return;
    pos = p->id % 4;
    maxx = p->maxX;
    maxy = p->maxY;
    if(pos==0 || pos==2){
        p->posY = p->minY+rand()%(maxy-p->minY);
        p->dirY = 0;
        if(pos == 0){
            p->posX = p->minX;
            p->dirX = 1;
        }
        else{
            p->posX = maxx-1;
            p->dirX = -1;
        }
    }
    else{
        p->posX = p->minX+rand()%(maxx-p->minX);
        p->dirX = 0;
        if(pos == 1){
            p->posY = maxy - 1;
            p->dirY = -1;
        }
        else{
            p->posY = p->minY;
            p->dirY = 1;
        }
    }
}





/**
 * participant_move:
 * 
 * Función que mueve a un participante según los datos que almacena.
 * 
 * In:  p: Participante a mover.
 */
int participant_move(Participant p){
    int r,posx,posy,maxx,maxy,minx,miny;
    if(p == NULL || p->particleNum==0)
        return 0;
    posx = p->posX;
    posy = p->posY;
    maxx = p->maxX;
    maxy = p->maxY;
    minx = p->minX;
    miny = p->minY;
    r = rand()%3;
    posx = posx + p->dirX;
    posy = posy + p->dirY;
    if((posx >= maxx || posx < minx) && (posy>=maxy || posy<miny)){
        p->dirX = -p->dirX;
        p->dirY = -p->dirY;
    }
    else if(posx >= maxx || posx < minx){
        p->dirX = -p->dirX;
        p->dirY = r-1;
    }
    else if(posy>=maxy || posy<miny){
        p->dirY = -p->dirY;
        p->dirX = r-1;
    }
    p->posX = p->posX + p->dirX;
    p->posY = p->posY + p->dirY;
    return 1;
}




/**
 * participant_particle_collided:
 * 
 * Función que edita los parámetros de un participante tal que el
 * participante colisionó dentro de la simulación.
 * 
 * In:  p: Participante.
 */
void participant_particle_collided(Participant p){
    if(p==NULL || p->particleNum<=0)
        return;
    p->particleNum = p->particleNum - 1;
    return;
}





/**
 * participante_get_x:
 * 
 * Función que entrega la posición del eje X del participante. Entrega
 * un valor -1 si el input no es válido.
 * 
 * In:  p: Participante 
 * 
 * Ou:  Posición actual en eje X.
 */
int participant_get_x(Participant p){
    return p==NULL?-1:p->posX;
}





/**
 * participante_get_y:
 * 
 * Función que entrega la posición del eje Y del participante. Entrega
 * un valor -1 si el input no es válido.
 * 
 * In:  p: Participante 
 * 
 * Ou:  Posición actual en eje Y.
 */
int participant_get_y(Participant p){
    return p==NULL?-1:p->posY;
}





/**
 * participante_get_max_x:
 * 
 * Función que entrega el límite del eje X del participante. Entrega
 * un valor -1 si el input no es válido.
 * 
 * In:  p: Participante 
 * 
 * Ou:  Límite del eje X para el participante.
 */
int participant_get_max_x(Participant p){
    return p==NULL?-1:p->maxX;
}





/**
 * participante_get_max_y:
 * 
 * Función que entrega el límite del eje Y del participante. Entrega
 * un valor -1 si el input no es válido.
 * 
 * In:  p: Participante 
 * 
 * Ou:  Límite del eje Y para el participante.
 */
int participant_get_max_y(Participant p){
    return p==NULL?-1:p->maxY;
}





/**
 * participante_get_id:
 * 
 * Función que entrega el id del participante. Entrega
 * un valor -1 si el input no es válido.
 * 
 * In:  p: Participante 
 * 
 * Ou:  ID del participante.
 */
int participant_get_id(Participant p){
    return p==NULL?-1:p->id;
}





/**
 * participant_get_particleNum:
 * 
 * Función que entrega el límite del eje X del participante. Entrega
 * un valor -1 si el input no es válido (simulando que no posee partículas).
 * 
 * In:  p: Participante 
 * 
 * Ou:  Cantidad de partículas del participante.
 */
int participant_get_particleNum(Participant p){
    return p==NULL?0:p->particleNum;
}





/**
 * participant_free:
 * 
 * Función que libera la memoria utilizada por un participante.
 * 
 * In:  p: Participante 
 */
void participant_free(Participant p){
    if(p!=NULL)
        free(p);
    return;
}