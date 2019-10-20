gcc main.c libraries/participant.c libraries/participant_list.c -lncurses -pthread &&\
./a.out 50 30 -1 10000;