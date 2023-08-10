#!/bin/sh

clear && c99 -fPIC -Wpedantic -pedantic-errors -Wextra -Wall -ggdb3 -O3 -o ./main3 ./main.c ./logging.c ./tplgymgt.c && time ./main
clear && lint ./main.c ./logging.c ./logging.h ./pddd.h ./tplgymgt.c ./tplgymgt.h | grep -vE 'reference base|means there|bug in Splint| 25 indirections'
clear && rm ./valgrind.txt; valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind.txt ./main && clear && less ./valgrind.txt
