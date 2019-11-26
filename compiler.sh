gcc main.c libraries/participant.c libraries/participant_list.c -Wall -lncurses -pthread &&\
./a.out 1000 10 -1 10000;