CC=gcc
CCOPTS=--std=gnu99 -Wall -g
AR=ar
HEADERS=top.h
OBJS=top.o
BINS=main

.phony: clean all

all: $(BINS)

%.o: %.c $(HEADERS)
	$(CC) $(CCOPTS) $(LDFLAGS) -c -o $@  $<

main: main.c $(OBJS) 
	$(CC) $(CCOPTS) $(LDFLAGS) -o top $^ 

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)