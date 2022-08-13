CC=gcc
CCOPTS=--std=c17 -Wall 
AR=ar
HEADERS=top_utils.h
OBJS=top_utils.o
BINS=main

.phony: clean all

all: $(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) $(LDFLAGS) -c -o $@  $<

main: main.c $(LIBS)
	$(CC) $(CCOPTS) $(LDFLAGS) -o top $^ 

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)