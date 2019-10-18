#include "participant.h"

typedef struct Participant_list * Participant_list;

extern Participant_list participant_list_create(int n,int numParticle, int maxx,int maxy);
extern int participant_list_collision(Participant_list listp,Participant p);
extern int participant_list_get_length(Participant_list listp);