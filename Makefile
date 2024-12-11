
CC=gcc
CFLAGS=-Wall -ggdb2
LIBS=-lpthread

all:  intersection

clean:
	rm intersection

intersection: intersection.c intersection_time.c intersection_time.h arrivals.h input.h
	$(CC) $(CFLAGS) -o intersection intersection.c intersection_time.c $(LIBS)
