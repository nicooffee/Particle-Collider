#include <stdio.h>
#include <stdlib.h>
#include "participant.h"
#include <time.h>

int main(){
    Participant p;
    srand((unsigned) time(NULL)); 
    p = participant_create(10,4,100,100);
    printf("%d %d %d %d\n",   participant_get_x(p),
                            participant_get_y(p),
                            participant_get_max_x(p),
                            participant_get_max_y(p));
    participant_move(p);
    printf("%d %d %d %d\n",   participant_get_x(p),
                            participant_get_y(p),
                            participant_get_max_x(p),
                            participant_get_max_y(p));    
    participant_free(p);
    return 0;

}