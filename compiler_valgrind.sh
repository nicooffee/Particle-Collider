gcc main.c libraries/participant.c libraries/participant_list.c -lncurses -pthread -ggdb3 &&\
valgrind --leak-check=full ./a.out 10 4 1000 10000 > log.txt 2>&1;