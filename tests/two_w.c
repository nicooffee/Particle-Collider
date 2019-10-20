#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#define DELAY 30000
#define ONE "EVADE EL METRO\0"

pthread_mutex_t mutex_pantalla=PTHREAD_MUTEX_INITIALIZER;

struct Message_w{
    WINDOW *w;
};

void init_setup();
void *foo(void *m);

int main(){
    WINDOW *w1,*w2;
    struct Message_w *wm1,*wm2;
    int maxX, maxY;
    pthread_t thread_w1,thread_w2;
    init_setup();
    getmaxyx(stdscr,maxY,maxX);
    w1 = newwin(maxY,maxX/2,0,0);
    w2 = newwin(maxY,maxX/2,0,maxX/2+1);
    wm1 = (struct Message_w*) calloc(1,sizeof(struct Message_w));
    wm2 = (struct Message_w*) calloc(1,sizeof(struct Message_w));
    wm1->w = w1;
    wm2->w = w2;
    pthread_create(&thread_w1,NULL,&foo,(void *) wm1);
    pthread_create(&thread_w2,NULL,&foo,(void *) wm2);
    pthread_join(thread_w1, NULL);
	pthread_join(thread_w2, NULL);
    return 0;
}


void *foo(void *m){
    WINDOW *w;
    int maxx, maxy, posx, posy, directionX,directionY;
    int r;
    w = ((struct Message_w *) m)->w;
    directionX = 1;
    directionY = 0;
    getmaxyx(w,maxy,maxx);
    posy = maxy/2;
    posx = maxx/2;
    while(1){
        wclear(w);
        //pthread_mutex_lock(&mutex_pantalla);
        wcolor_set(w,rand()%14+1, NULL);
        mvwprintw(w,posy,posx,ONE);       
        wrefresh(w);
        //pthread_mutex_unlock(&mutex_pantalla);
        usleep(DELAY);
        posx = posx + directionX;
        posy = posy + directionY;
        r = rand()%3;
        if((posx >= (maxx-strlen(ONE)) || posx < 0) && (posy>=maxy || posy<0)){
            directionX = -directionX;
            directionY = -directionY;
        }
        else if(posx >= (maxx-strlen(ONE)) || posx < 0){
            directionX = -directionX;
            directionY = r-1;
        }
        else if(posy>=maxy || posy<0){
            directionY = -directionY;
            directionX = r-1;
        }
    }
    delwin(w);
}

void init_setup(){
    initscr();
    srand((unsigned) time(NULL));   
    noecho();
    start_color();
    init_pair(1,COLOR_RED,COLOR_BLACK);
    init_pair(2,COLOR_GREEN,COLOR_BLACK);
    init_pair(3,COLOR_YELLOW,COLOR_BLACK);
    init_pair(4,COLOR_BLUE,COLOR_BLACK);
    init_pair(5,COLOR_MAGENTA,COLOR_BLACK);
    init_pair(6,COLOR_CYAN,COLOR_BLACK);
    init_pair(7,COLOR_BLUE,COLOR_WHITE);
    init_pair(8,COLOR_WHITE,COLOR_RED);
    init_pair(9,COLOR_BLACK,COLOR_GREEN);
    init_pair(10,COLOR_BLUE,COLOR_YELLOW);
    init_pair(11,COLOR_WHITE,COLOR_BLUE);
    init_pair(12,COLOR_WHITE,COLOR_MAGENTA);
    init_pair(13,COLOR_BLACK,COLOR_CYAN);
    curs_set(FALSE);
}
