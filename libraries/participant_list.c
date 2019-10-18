#include <stdlib.h>
#include "participant_list.h"
#include "participant.h"

struct Participant_list{
    Participant *list;
    int numParticipant;
};


Participant_list participant_list_create(int n,int numParticle, int maxx,int maxy){
    int i=0;
    struct Participant_list *new;
    new = (struct Participant_list *) calloc(1,sizeof(struct Participant_list));
    new->list = (Participant *) calloc(n,sizeof(Participant));
    for(;i<n;i++){
        new->list[i] = participant_create(numParticle,i,maxx,maxy);
        while(participant_list_collision(new,new->list[i])){
            participant_free(new->list[i]);
            new->list[i] = participant_create(numParticle,i,maxx,maxy);
        }
    }
    return new;
}

int participant_list_collision(Participant_list listp,Participant p){
    int i = 0;
    int posX = participant_get_x(p);
    int posY = participant_get_y(p);
    int id = participant_get_id(p);
    for(;i<participant_list_get_length(listp);i++)
        if( participant_get_x(listp->list[i])==posX && \
            participant_get_y(listp->list[i])==posY && \
            participant_get_id(listp->list[i])!=id)
            return 1;
    return 0;
} 


int participant_list_get_length(Participant_list listp){
    return listp->numParticipant;
}