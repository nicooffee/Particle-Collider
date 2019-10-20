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
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "libraries/participant_list.h"
#include "libraries/participant.h"

/**-------------------------------------------ESTRUCTURAS------------------------------------------------------------**/
/**
 * struct Message_w
 * Estructura para enviar los datos que utilizan las funciones del manejo de la ventana 1.
 * 
 * listp    : Lista de participantes.
 * w        : ventana donde se muestran los participantes.
 * delay    : Delay de refresco de la ventana.
 */
struct Message_w{
    Participant_list listp;
    WINDOW *w1;
    WINDOW *w2;
    int delay;
    int maxInstant;
    struct Colision **colisiones;
};


/**
 * Estructura para enviar los datos que utilizan las funciones del movimiento de un participante.
 * 
 * listp        : Lista de participantes.
 * id           : ID del participante a manejar
 * delay        : Delay del participante.
 * maxInstant   : Máximo de instantes permitidos en la simulación.
 */
struct Message_p{
    Participant_list listp;
    Participant p;
    int delay;
    int maxInstant;
    struct Colision **colisiones;
};


/**
 * Estructura para almacenar los datos de una colisión
 * 
 * instante : Instante en el que ocurre la colisión
 * posX     : Eje x del lugar de colisión
 * posY     : Eje y del lugar de colisión
 * id1      : ID del participante que genera la colisión
 * id2      : ID del participante que recibe la colisión
 */
struct Colision{
    int instante;
    int posX;
    int posY;
    int id1;
    int id2;
};
/**-----------------------------------------------PROTOTIPOS---------------------------------------------------------**/
void init_setup();
void print_participant(Participant p, WINDOW *w);
void *print_participant_list(void *message);
void print_info(WINDOW *w,struct Colision **colisiones);
void *move_participant_list(void * message);
struct Message_p *create_message_p(Participant_list listp,Participant p,int delay,int maxInstantm,struct Colision **colisiones);
struct Message_w *create_message_w(Participant_list listp, WINDOW *w1,WINDOW *w2,int d,int i,struct Colision **colisiones);
struct Colision *create_colision(int inst,int posx,int posy,int id1,int id2);


/**-------------------------------------------VARIABLES GLOBALES-----------------------------------------------------**/
int INSTANT = 0;                                                //Instante de la simulación.
int COLISIONES = 0;                                             //Cantidad de colisiones total.
int first_coll;                                                 //Posición de la última colisión
int length_coll;                                                //Cantidad de colisiones a guardar (las n ultimas).
pthread_mutex_t mutex_screen=PTHREAD_MUTEX_INITIALIZER;         //Mutex para sincronizar las ventanas.
pthread_mutex_t mutex_participant=PTHREAD_MUTEX_INITIALIZER;    //Mutex para sincronizar el uso de lista de participantes.
/**
 * Parametros de inicio:
 *  - n:    Cantidad de particulas de cada participante.
 *  - k:    Cantidad de participantes.
 *  - i:    Iteraciones del proceso.
 *  - d:    Delay de refresco de ventana.
 */
int main(int args, char **argv){
    pthread_t thread_screen1,*thread_participant;
    struct Message_w *screen_message;
    struct Message_p **participant_message;
    struct Colision **colisiones=NULL;
    Participant_list list;
    WINDOW *w_particle,*w_data;
    int n,k,i,d;
    int j;
    int maxx,maxy;
    int maxx_p,maxy_p,maxx_d,maxy_d;
    if(args < 5){
        printf("Parametros de ejecucion incorrectos.");
        exit(0);
    }
    init_setup();
    getmaxyx(stdscr,maxy,maxx);
    w_particle = newwin(maxy,maxx*5/10,0,0);
    w_data = newwin(maxy,maxx*5/10,0,maxx*5/10);
    getmaxyx(w_particle,maxy_p,maxx_p);
    getmaxyx(w_particle,maxy_d,maxx_d);
    n = atoi(argv[1]);
    k = atoi(argv[2]);
    i = atoi(argv[3]);
    d = atoi(argv[4]);
    first_coll = 0;
    length_coll = maxy/2-4;
    colisiones = (struct Colision**) calloc(length_coll,sizeof(struct Colision*));
    list = participant_list_create(k,n,maxx_p-1,maxy_p-1,1,1);
    screen_message =        create_message_w(list,w_particle,w_data,d,i,colisiones);
    thread_participant =    (pthread_t*) calloc(k,sizeof(pthread_t));
    participant_message =   (struct Message_p**) calloc(k,sizeof(struct Message_p*));
    pthread_create(&thread_screen1,NULL,&print_participant_list,(void *) screen_message);
    for(j=0;j<k;j++){
        participant_message[j] = create_message_p(list,participant_list_get(list,j),d,i,colisiones);
        pthread_create(&thread_participant[j],NULL,&move_participant_list,(void *)participant_message[j]);
    }
    pthread_join(thread_screen1, NULL);
    for(j=0;j<k;j++)
        pthread_join(thread_participant[j],NULL);
    participant_list_free(list);
    pthread_mutex_destroy(&mutex_screen);
    pthread_mutex_destroy(&mutex_participant);
    free(thread_participant);
    free(screen_message);
    for(j=0;j<k;j++)
        free(participant_message[j]);
    for(j=0;j<length_coll;j++)
        if(colisiones[j]!=NULL)
            free(colisiones[j]);
    free(colisiones);
    free(participant_message);
    delwin(w_data);
    delwin(w_particle);
    endwin();
    return 0;
}





/**
 * move_participant_list:
 * 
 * Función que mueve al participante según los datos del mensaje. Se utiliza
 * un mutex para asegurar la exclusión mutua al mover o colisionar con otra
 * partícula. Esta función finaliza su ejecución cuando el participante no posee
 * partículas o cuando el instante de la simulación supera al máximo definido.
 */
void *move_participant_list(void * message){
    Participant aux=NULL;
    Participant p;
    Participant_list listp;
    int delay;
    int maxInstant;
    int flag=1;
    int inst;
    struct Colision **colisiones=NULL;
    if(message==NULL) return NULL;
    listp = ((struct Message_p*) message)->listp;
    delay = ((struct Message_p*) message)->delay;
    maxInstant = ((struct Message_p*) message)->maxInstant;
    colisiones = ((struct Message_p*) message)->colisiones;
    p = ((struct Message_p*) message)->p;
    while(flag!=0 && (maxInstant==-1 || INSTANT<=maxInstant)){
        pthread_mutex_lock(&mutex_participant);
        flag=participant_list_move(listp,p,&aux);
        inst = INSTANT;
        INSTANT = INSTANT + 1;
        if(aux != NULL){
            first_coll = ((first_coll - 1)<0?length_coll+(first_coll - 1):first_coll - 1)% length_coll;
            if(colisiones[first_coll]!=NULL) free(colisiones[first_coll]);
            colisiones[first_coll] = \
                create_colision(inst,\
                                participant_get_x(p),\
                                participant_get_y(p),\
                                participant_get_id(p),\
                                participant_get_id(aux));
            COLISIONES = COLISIONES + 1;
            aux=NULL;
        }
        pthread_mutex_unlock(&mutex_participant);
        usleep(delay);
    }
    return NULL;
}





/**
 * print_participant_list:
 * 
 * Función que imprime los datos de una lista de participantes dentro
 * de una ventana, según corresponda a los datos del mensaje. Se utilizan
 * dos mutex, uno para asegurar el uso único de la ventana y el otro para
 * asegurar el uso único de la lista de participantes. 
 */
void *print_participant_list(void *message){
    int j,flg,delay,maxInstant;
    Participant_list listp=((struct Message_w*) message)->listp;
    Participant aux;
    WINDOW *w1=((struct Message_w*) message)->w1;
    WINDOW *w2=((struct Message_w*) message)->w2;
    delay=((struct Message_w*) message)->delay;
    maxInstant = ((struct Message_w*) message)->maxInstant;
    while((maxInstant==-1 || INSTANT<=maxInstant)){
        box(w1,'*','*');
        flg = first_coll;
        pthread_mutex_lock(&mutex_screen);
        pthread_mutex_lock(&mutex_participant);
        for(j=0;j<participant_list_get_length(listp);j++){
            aux = participant_list_get(listp,j);
            if(participant_get_particleNum(aux)!=0){
                print_participant(aux,w1);
                wnoutrefresh(w1);
            }
        }
        print_info(w2,((struct Message_w*) message)->colisiones);
        pthread_mutex_unlock(&mutex_screen);
        pthread_mutex_unlock(&mutex_participant);
        doupdate();
        usleep(delay/2);
        wclear(w1);
        wclear(w2);
    }
}





/**
 * print_info:
 * 
 * Función que imprime los datos de las ultimas length_coll colisiones
 * dentro de la simulación en la pantalla w. Además muestra datos globales
 * relacionados con la simulación.
 */
void print_info(WINDOW *w,struct Colision **colisiones){
    int line;
    int j;
    int maxx,maxy;
    char *buffer=(char*) calloc(100,sizeof(char));
    struct Colision *aux_col;
    box(w,'*','*');
    wnoutrefresh(w);
    line = 3;
    getmaxyx(w,maxy,maxx);
    sprintf(buffer,"| Instante: %10d| C. Colisiones: %10d",INSTANT,COLISIONES);
    mvwhline(w,line,1,' ',maxx-2);
    mvwprintw(w,1,1,buffer);
    wnoutrefresh(w);
    j = first_coll;
    do{
        if(colisiones[j]!=NULL){
            aux_col = colisiones[j];
            sprintf(buffer,"| Colision-> I: %10d|(x,y): (%4d,%4d)| %2d -><- %2d",aux_col->instante,aux_col->posX,aux_col->posY,aux_col->id1,aux_col->id2);
            mvwhline(w,line,1,' ',maxx-2);
            mvwprintw(w,line,1,buffer);
            strcpy(buffer,"\0");
            wnoutrefresh(w);
            line+=2;
        }
        j=(j+1)%length_coll; 
    }while(j!=first_coll && colisiones[j]!=NULL);
    free(buffer);
}




/**
 * print_participant:
 * 
 * Función utilizada por print_participant_list. Imprime la posición
 * de un participante en la ventana w.
 */
void print_participant(Participant p, WINDOW *w){
    wattron(w,COLOR_PAIR(participant_get_id(p)%14+1));
    mvwprintw(w,participant_get_y(p),participant_get_x(p),"*");
    wattroff(w,COLOR_PAIR(participant_get_id(p)%13+1));
}





/**
 * init_setup:
 * 
 * Función para inicializar las ventanas y números aleatorios.
 */
void init_setup(){
    initscr();
    srand((unsigned) time(NULL));   
    noecho();
    start_color();
    init_pair(1,COLOR_RED,COLOR_RED);
    init_pair(2,COLOR_GREEN,COLOR_GREEN);
    init_pair(3,COLOR_YELLOW,COLOR_YELLOW);
    init_pair(4,COLOR_CYAN,COLOR_CYAN);
    init_pair(5,COLOR_MAGENTA,COLOR_MAGENTA);
    init_pair(6,COLOR_WHITE,COLOR_WHITE);
    init_pair(7,COLOR_BLUE,COLOR_BLUE);

    init_pair(8,COLOR_WHITE,COLOR_RED);
    init_pair(9,COLOR_WHITE,COLOR_GREEN);
    init_pair(10,COLOR_WHITE,COLOR_YELLOW);
    init_pair(11,COLOR_WHITE,COLOR_CYAN);
    init_pair(12,COLOR_WHITE,COLOR_MAGENTA);
    init_pair(13,COLOR_WHITE,COLOR_WHITE);
    init_pair(14,COLOR_WHITE,COLOR_BLUE);
    curs_set(FALSE);
}


struct Message_p *create_message_p(Participant_list listp,Participant p,int delay,int maxInstant,struct Colision **colisiones){
    struct Message_p *participant_message = (struct Message_p*) calloc(1,sizeof(struct Message_p));
    participant_message->listp = listp;
    participant_message->p = p;
    participant_message->delay = delay;
    participant_message->maxInstant = maxInstant;
    (* participant_message).colisiones = colisiones;
    return participant_message;
}

struct Message_w *create_message_w(Participant_list listp, WINDOW* w1, WINDOW *w2,int d,int i,struct Colision **colisiones){
    struct Message_w *screen_message = (struct Message_w*) calloc(1,sizeof(struct Message_w));
    screen_message->listp = listp;
    screen_message->w1 = w1;
    screen_message->w2 = w2;
    screen_message->delay = d;
    screen_message->maxInstant = i;
    (* screen_message).colisiones = colisiones;
    return screen_message;
}

struct Colision *create_colision(int inst,int posx,int posy,int id1,int id2){
    struct Colision *new = (struct Colision*) calloc(1,sizeof(struct Colision));
    new->instante = inst;
    new->posX = posx;
    new->posY = posy;
    new->id1 = id1;
    new->id2 = id2;
    return new;
}