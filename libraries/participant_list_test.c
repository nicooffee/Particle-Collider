#include <stdio.h>
#include <unistd.h>
#include "participant_list.h"
#include "participant.h"

int main(){
    int i=0;
    int n=10;
    Participant_list lista;
    Participant aux;
    Participant col=NULL;
    lista = participant_list_create( n, 10, 100,100,0,0);
    for(;i<n;i++){
        aux = participant_list_get(lista,i);
        printf("h: %d|%d %d\t", participant_get_id(aux),\
                                participant_get_x(aux),\
                                participant_get_y(aux));
        participant_list_move(lista,aux,&col);
        printf("m: %d %d\n",   participant_get_x(aux),\
                            participant_get_y(aux));
    }
    while(1){
        aux = participant_list_get(lista,0);
        participant_list_move(lista,aux,&col);
        printf("m: %d %d p: %d\n",  participant_get_x(aux),\
                                    participant_get_y(aux),\
                                    participant_get_particleNum(aux)    );
        col=NULL;
        usleep(50000);
    }
    return 0;
}