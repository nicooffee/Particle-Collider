gcc main.c libraries/participant.c libraries/participant_list.c -Wall -lncurses -pthread &&\
./a.out 10000000 50 -1 40000;