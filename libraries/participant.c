#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "participant.h"

struct Participant{
    int id;
    int particleNum;
    int posX;
    int posY;
    int maxX;
    int maxY;
    int dirX;
    int dirY;
};
/**
 * generar otra estructura que controel a los jugadores,
 * para evitar problemas de movimiento.
 * 
 * El jugador es independiente a si hay jugadores en la
 * posición donde se pondrá.
 */

Participant participant_create(int parNum, int participant , int maxx, int maxy){
    struct Participant *new;
    int pos = participant%4;
    new = (struct Participant*) calloc(1,sizeof(struct Participant));
    new->id = participant;
    new->particleNum = parNum;
    new->maxX = maxx;
    new->maxY = maxy;
    if(pos==0 || pos==2){
        new->posY = rand()%maxy;
        new->dirY = 0;
        if(pos == 0){
            new->posX = 0;
            new->dirX = 1;
        }
        else{
            new->posX = maxx-1;
            new->dirX = -1;
        }
    }
    else{
        new->posX = rand()%maxx;
        new->dirX = 0;
        if(pos == 1){
            new->posY = maxy - 1;
            new->dirY = -1;
        }
        else{
            new->posY = 0;
            new->dirY = 1;
        }
    }
    return new;
}

void participant_move(Participant p){
    int r,posx,posy,maxx,maxy;
    if(p == NULL)
        return;
    posx = p->posX;
    posy = p->posY;
    maxx = p->maxX;
    maxy = p->maxY;
    r = rand()%3;
    posx = posx + p->dirX;
    posy = posy + p->dirY;
    if((posx >= maxx || posx < 0) && (posy>=maxy || posy<0)){
        p->dirX = -p->dirX;
        p->dirY = -p->dirY;
    }
    else if(posx >= maxx || posx < 0){
        p->dirX = -p->dirX;
        p->dirY = r-1;
    }
    else if(posy>=maxy || posy<0){
        p->dirY = -p->dirY;
        p->dirX = r-1;
    }
    p->posX = p->posX + p->dirX;
    p->posY = p->posY + p->dirY;
    return;
}
void participant_particle_sub(Participant p){
    if(p==NULL)
        return;
    if(p->particleNum>0)
        p->particleNum = p->particleNum - 1;
    return;
}

int participant_get_x(Participant p){
    return p==NULL?-1:p->posX;
}

int participant_get_y(Participant p){
    return p==NULL?-1:p->posY;
}

int participant_get_max_x(Participant p){
    return p==NULL?-1:p->maxX;
}

int participant_get_max_y(Participant p){
    return p==NULL?-1:p->maxY;
}
int participant_get_id(Participant p){
    return p==NULL?-1:p->id;
}

void participant_free(Participant p){
    if(p!=NULL)
        free(p);
    return;
}