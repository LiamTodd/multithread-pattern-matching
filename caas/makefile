all: serial_kmp parallel_kmp

CC = gcc

serial_kmp: ./src/serial.c ./src/generic_helpers.c ./src/serial_helpers.c ./src/headerfiles/generic_helpers.h ./src/headerfiles/serial_helpers.h
	$(CC) ./src/serial.c ./src/generic_helpers.c ./src/serial_helpers.c -o serial_kmp

parallel_kmp: ./src/parallel.c ./src/generic_helpers.c ./src/parallel_helpers.c ./src/headerfiles/generic_helpers.h ./src/headerfiles/parallel_helpers.h
	$(CC) ./src/parallel.c ./src/generic_helpers.c ./src/parallel_helpers.c -fopenmp -o parallel_kmp