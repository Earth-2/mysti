CC=gcc

mysti: main.c statbar.c mysti.h
	$(CC) -Wall -lmenu -lncurses -o mysti main.c
