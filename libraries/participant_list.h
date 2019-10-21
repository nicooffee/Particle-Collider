#include "participant.h"

typedef struct Participant_list * Participant_list;

extern Participant_list participant_list_create(int n,int numParticle, int maxx,int maxy,int minx,int miny);
extern int participant_list_move(Participant_list listp,Participant p,Participant *coll);
extern Participant participant_list_collision(Participant_list listp,Participant p);
extern int participant_list_get_length(Participant_list listp);
extern Participant participant_list_get(Participant_list listp, int id);
extern int participant_list_get_numActive(Participant_list listp);
extern void participant_list_free(Participant_list listp);