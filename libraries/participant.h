
typedef struct Participant * Participant;

extern Participant participant_create(int parNum, int participant , int maxx, int maxy);
extern void participant_move(Participant p);
extern void participant_particle_sub(Participant p);
extern int participant_get_x(Participant p);
extern int participant_get_y(Participant p);
extern int participant_get_max_x(Participant p);
extern int participant_get_max_y(Participant p);
extern int participant_get_id(Participant p);
extern void participant_free(Participant p);