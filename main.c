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

#define ROJO        1
#define VERDE       2
#define AMARILLO    3
#define CYAN        4
#define MAGENTA     5
#define BLANCO      6
#define AZUL        7
#define BLANCO_ROJO     8
#define BLANCO_VERDE    9
#define BLANCO_AMARILLO 10
#define BLANCO_CYAN     11
#define BLANCO_MAGENTA  12
#define BLANCO_NEGRO    13
#define BLANCO_AZUL     14

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
void *print_window(void *message);
void *move_participant_list(void * message);
struct Message_p *create_message_p(Participant_list listp,Participant p,int delay,int maxInstantm,struct Colision **colisiones);
struct Message_w *create_message_w(Participant_list listp, WINDOW *w1,WINDOW *w2,int d,int i,struct Colision **colisiones);
struct Colision *create_colision(int inst,int posx,int posy,int id1,int id2);
void *stop_ejec(void *message);


void *print_info(void *message);
void *print_colisiones(void *message);
void *print_participant_list(void *message);
void set_frame_w1(WINDOW *w);
void set_frame_w2(WINDOW *w);
void set_msg_w2(WINDOW *w,int maxInstant);

/**-------------------------------------------VARIABLES GLOBALES-----------------------------------------------------**/
int INSTANT = 0;                                                //Instante de la simulación.
int COLISIONES = 0;                                             //Cantidad de colisiones total.
int first_coll;                                                 //Posición de la última colisión
int length_coll;                                                //Cantidad de colisiones a guardar (las n ultimas).
pthread_mutex_t mutex_screen=PTHREAD_MUTEX_INITIALIZER;         //Mutex para sincronizar las ventanas.
pthread_mutex_t mutex_participant=PTHREAD_MUTEX_INITIALIZER;    //Mutex para sincronizar el uso de lista de participantes.
pthread_mutex_t mutex_screen_2=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond_colision=PTHREAD_COND_INITIALIZER;
int ejec_flag = 1;
/**
 * Parametros de inicio:
 *  - n:    Cantidad de particulas de cada participante.
 *  - k:    Cantidad de participantes.
 *  - i:    Iteraciones del proceso.
 *  - d:    Delay de refresco de ventana.
 */
int main(int args, char **argv){
    pthread_t thread_screen1,*thread_participant,thread_ejec;
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
    length_coll = maxy-13;
    colisiones = (struct Colision**) calloc(length_coll,sizeof(struct Colision*));
    list = participant_list_create(k,n,maxx_p-1,maxy_p-1,1,1);
    screen_message =        create_message_w(list,w_particle,w_data,d,i,colisiones);
    thread_participant =    (pthread_t*) calloc(k,sizeof(pthread_t));
    participant_message =   (struct Message_p**) calloc(k,sizeof(struct Message_p*));
    pthread_create(&thread_screen1,NULL,&print_window,(void *) screen_message);
    pthread_create(&thread_ejec,NULL,&stop_ejec,(void *) screen_message);
    for(j=0;j<k;j++){
        participant_message[j] = create_message_p(list,participant_list_get(list,j),d,i,colisiones);
        pthread_create(&thread_participant[j],NULL,&move_participant_list,(void *)participant_message[j]);
    }
    pthread_join(thread_screen1, NULL);
    pthread_join(thread_ejec,NULL);
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
    struct Colision **colisiones=NULL;
    if(message==NULL) return NULL;
    listp           = ((struct Message_p*) message)->listp;
    delay           = ((struct Message_p*) message)->delay;
    maxInstant      = ((struct Message_p*) message)->maxInstant;
    colisiones      = ((struct Message_p*) message)->colisiones;
    p               = ((struct Message_p*) message)->p;
    while(flag!=0 && (maxInstant==-1 || INSTANT<=maxInstant) && ejec_flag){
        pthread_mutex_lock(&mutex_participant);
        flag=participant_list_move(listp,p,&aux);
        if(aux != NULL){
            first_coll = ((first_coll - 1)<0?length_coll+(first_coll - 1):first_coll - 1)% length_coll;
            if(colisiones[first_coll]!=NULL) free(colisiones[first_coll]);
            colisiones[first_coll] = \
                            create_colision(INSTANT,\
                                participant_get_x(p),\
                                participant_get_y(p),\
                                participant_get_id(p),\
                                participant_get_id(aux));
            COLISIONES = COLISIONES + 1;
            pthread_cond_signal(&cond_colision);
            aux=NULL;
        }
        INSTANT = INSTANT + 1;
        pthread_mutex_unlock(&mutex_participant);
        usleep(delay);
    }
    return NULL;
}





/**
 * print_window:
 * 
 * Función que imprime los datos de una lista de participantes dentro
 * de una ventana, según corresponda a los datos del mensaje. Se utilizan
 * dos mutex, uno para asegurar el uso único de la ventana y el otro para
 * asegurar el uso único de la lista de participantes. 
 */
//create_message_w(Participant_list listp, WINDOW* w1, WINDOW *w2,int d,int i,struct Colision **colisiones
/*
pthread_mutex_t mutex_screen_2=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond_colision=PTHREAD_COND_INITIALIZER;*/

void *print_window(void *message){
    pthread_t w1_tabl_pth,w2_col_pth,w2_info_pth;
    struct Message_w *m_pl,*m_col,*m_info;
    int delay,maxInstant,w1_mx,w1_my,w2_mx,w2_my;
    Participant_list listp;
    struct Colision **colisiones;
    WINDOW *w1,*w2,*w1_tablero,*w2_colisiones,*w2_mensaje,*w2_info;
    listp       = ((struct Message_w*) message)->listp;
    w1  = ((struct Message_w*) message)->w1;
    w2  = ((struct Message_w*) message)->w2;
    delay       = ((struct Message_w*) message)->delay;
    maxInstant  = ((struct Message_w*) message)->maxInstant;
    colisiones  = ((struct Message_w*) message)->colisiones;
    getmaxyx(w1,w1_my,w1_mx);
    getmaxyx(w2,w2_my,w2_mx);
    w1_tablero      = derwin(w1,w1_my-2,w1_mx-2,1,1);
    w2_mensaje      = derwin(w2,5,w2_mx-3,w2_my-6,2);
    w2_info         = derwin(w2,4,w2_mx-3,1,2);
    w2_colisiones   = derwin(w2,w2_my-13,w2_mx-3,6,2);
    set_frame_w1(w1);
    set_frame_w2(w2);
    set_msg_w2(w2_mensaje,maxInstant);
    m_pl    = create_message_w(listp,w1_tablero,NULL,delay,maxInstant,NULL);
    m_col   = create_message_w(NULL,w2_colisiones,NULL,delay,maxInstant,colisiones);
    m_info  = create_message_w(listp,w2_info,NULL,delay,maxInstant,NULL);
    pthread_create(&w1_tabl_pth,NULL,&print_participant_list,(void *) m_pl);
    pthread_create(&w2_col_pth,NULL,&print_colisiones,(void *) m_col);
    pthread_create(&w2_info_pth,NULL,&print_info,(void *)m_info);
    pthread_join(w2_col_pth, NULL);
    pthread_join(w1_tabl_pth, NULL);
    pthread_join(w2_info_pth,NULL);
    mvwhline(w2_mensaje,0,0,' ',w2_mx-3);
    mvwhline(w2_mensaje,1,0,' ',w2_mx-3);
    mvwprintw(w2_mensaje,0,0," Simulacion ejecutada con exito.");
    mvwprintw(w2_mensaje,1,0," Presione ESC para salir.");
    wrefresh(w2_mensaje);
    while(wgetch(w2_mensaje)!=27);
    free(m_pl);
    free(m_col);
    free(m_info);
    delwin(w1_tablero);
    delwin(w2_mensaje);
    delwin(w2_colisiones);
}

void *print_participant_list(void *message){
    int j,delay;
    Participant aux;
    WINDOW *w = ((struct Message_w*) message)->w1;
    Participant_list listp=((struct Message_w*) message)->listp;
    delay = ((struct Message_w*) message)->delay;
    while(ejec_flag){
        wclear(w);
        pthread_mutex_lock(&mutex_screen);
        pthread_mutex_lock(&mutex_participant);
        for(j=0;j<participant_list_get_length(listp);j++){
            aux = participant_list_get(listp,j);
            if(participant_get_particleNum(aux)!=0)
                print_participant(aux,w);
        }
        wnoutrefresh(w);
        pthread_mutex_unlock(&mutex_screen);
        pthread_mutex_unlock(&mutex_participant);
        usleep(delay);
    } 
}
/**
 * print_colisiones:
 * 
 * Función que imprime los datos de las ultimas length_coll colisiones
 * dentro de la simulación en la pantalla w. 
 */
void *print_colisiones(void *message){
    int line,j,maxx,maxy,delay;
    struct Colision *aux_col;
    WINDOW *w = ((struct Message_w*) message)->w1;
    struct Colision **colisiones = ((struct Message_w*) message)->colisiones;
    delay = ((struct Message_w*) message)->delay;
    getmaxyx(w,maxy,maxx);
    while(ejec_flag){
        pthread_mutex_lock(&mutex_screen);
        pthread_cond_wait(&cond_colision,&mutex_screen);
        j = first_coll;
        line = 0;
        wclear(w);
        do{
            if(colisiones[j]!=NULL){
                aux_col = colisiones[j];
                mvwhline(w,line,0,' ',maxx-3);
                mvwprintw(w,line,0," Colision -> I: %8d|(x,y): (%3d,%3d)| %2d -><- %2d",aux_col->instante,aux_col->posX,aux_col->posY,aux_col->id1,aux_col->id2);
                line+=1;
            }
            j=(j+1)%length_coll; 
        }while(j!=first_coll && colisiones[j]!=NULL);
        wnoutrefresh(w);
        pthread_mutex_unlock(&mutex_screen);
        usleep(delay);
    }
    return NULL;
}

void *print_info(void *message){
    int maxx,maxy,delay;
    WINDOW *w = ((struct Message_w*) message)->w1;
    Participant_list listp = ((struct Message_w*) message)->listp;
    delay = ((struct Message_w*) message)->delay;
    getmaxyx(w,maxy,maxx);
    while(ejec_flag){
        pthread_mutex_lock(&mutex_screen);
        mvwhline(w,1,0,' ',maxx);
        mvwprintw(w,1,0," Instante actual: %10d",INSTANT);
        mvwhline(w,2,0,' ',maxx);
        mvwprintw(w,2,0," Colisiones: %5d| Participantes restantes: %2d",COLISIONES,participant_list_get_numActive(listp));
        wnoutrefresh(w);
        doupdate();
        pthread_mutex_unlock(&mutex_screen);
        usleep(delay);
    }
    return NULL;
}




/**
 * stop_ejec:
 * 
 * Funcion para finalizar la ejecución general de las demás hebras.
 */
void *stop_ejec(void *message){
    WINDOW *w2  = ((struct Message_w*) message)->w2;
    while(wgetch(w2)!=27);
    pthread_cond_signal(&cond_colision);
    ejec_flag = 0;
}





/**
 * print_participant:
 * 
 * Función utilizada por print_window. Imprime la posición
 * de un participante en la ventana w.
 */
void print_participant(Participant p, WINDOW *w){
    wattron(w,COLOR_PAIR(participant_get_id(p)%13+1));
    mvwprintw(w,participant_get_y(p),participant_get_x(p),"*");
    wattroff(w,COLOR_PAIR(participant_get_id(p)%13+1));
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




/**-----------------------------------------INICIALIZADORES----------------------------------------------------------**/
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
    init_pair(ROJO,COLOR_RED,COLOR_RED);
    init_pair(VERDE,COLOR_GREEN,COLOR_GREEN);
    init_pair(AMARILLO,COLOR_YELLOW,COLOR_YELLOW);
    init_pair(CYAN,COLOR_CYAN,COLOR_CYAN);
    init_pair(MAGENTA,COLOR_MAGENTA,COLOR_MAGENTA);
    init_pair(BLANCO,COLOR_WHITE,COLOR_WHITE);
    init_pair(AZUL,COLOR_BLUE,COLOR_BLUE);
    init_pair(BLANCO_ROJO,COLOR_WHITE,COLOR_RED);
    init_pair(BLANCO_VERDE,COLOR_WHITE,COLOR_GREEN);
    init_pair(BLANCO_AMARILLO,COLOR_WHITE,COLOR_YELLOW);
    init_pair(BLANCO_CYAN,COLOR_WHITE,COLOR_CYAN);
    init_pair(BLANCO_MAGENTA,COLOR_WHITE,COLOR_MAGENTA);
    init_pair(BLANCO_NEGRO,COLOR_WHITE,COLOR_BLACK);
    init_pair(BLANCO_AZUL,COLOR_WHITE,COLOR_BLUE);
    curs_set(FALSE);
}
void set_frame_w1(WINDOW *w){
    wattron(w,COLOR_PAIR(BLANCO));
    box(w,'*','*');
    wattroff(w,COLOR_PAIR(BLANCO));
    wnoutrefresh(w);
}
void set_frame_w2(WINDOW *w){
    int maxy,maxx;
    getmaxyx(w,maxy,maxx);
    mvwvline(w,1,1,'|',maxy-2);
    wattron(w,COLOR_PAIR(BLANCO_CYAN));
    box(w,'*','*');
    wattroff(w,COLOR_PAIR(BLANCO_CYAN));
    mvwhline(w,5,2,'-',maxx-3);
    wattron(w,COLOR_PAIR(BLANCO_VERDE));
    mvwhline(w,maxy-7,1,'-',maxx-2);
    wattroff(w,COLOR_PAIR(BLANCO_VERDE));
    wnoutrefresh(w);
}
void set_msg_w2(WINDOW *w,int maxInstant){
    mvwprintw(w,0,0," Ejecucion en modo: ");
    if(maxInstant==-1){
        wattron(w,COLOR_PAIR(BLANCO_ROJO));
        wprintw(w,"indefinido.");
        wattroff(w,COLOR_PAIR(BLANCO_ROJO));
    } 
    else{
        wattron(w,COLOR_PAIR(BLANCO_VERDE));
        wprintw(w,"limitado");
        wattroff(w,COLOR_PAIR(BLANCO_VERDE));
        wprintw(w,", en %d iteraciones.",maxInstant);
    }
    mvwprintw(w,1,0," Presione ESC para detener.");
    wnoutrefresh(w);
}