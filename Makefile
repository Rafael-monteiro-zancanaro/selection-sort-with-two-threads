CC = gcc
FLAGS = -Wall -Wextra
OUTPUT = selsort_2_threads
IMPORTS = -lm #math.h
main:
	${CC} ${FLAGS} main.c -o ${OUTPUT} ${IMPORTS}

clear:
	rm -rf selsort_2_threads
