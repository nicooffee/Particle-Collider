/**********************************************************
 * 
 * example3.c
 * 
 * Programmer: Ruben Carvajal Schiaffino
 * 
 * Santiago de Chile, 14/10/2019
 * 
 * **********************************************************/

#include <ncurses.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#define ONE   "por la chucha que hace calor"
#define DELAY  30000


int main() {

   WINDOW *w;
   int maxx, maxy, posx, posy, directionX,directionY;
   int r;
   posx = 0; 
   posy = 15;
   directionX = 1;
   directionY = 0;
   srand((unsigned) time(NULL));
   initscr();
   getmaxyx(stdscr,maxy,maxx);
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
   while (1) {
      clear();               
      color_set(rand()%14+1, NULL);
      mvprintw(posy,posx,ONE);        
      refresh();
      usleep(DELAY);          
      posx = posx + directionX;
      posy = posy + directionY;
      //maxx, maxy
      r = rand()%3;
      if(posx >= maxx || posx < 0){
         directionX = -directionX;
         directionY = r-1;
      }
      else if(posy>=maxy || posy<0){
         directionY = -directionY;
         directionX = r-1;
      }
   }
   endwin();
}
