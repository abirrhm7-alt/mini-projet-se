CC = gcc
CFLAGS = -Wall -I.

all: scheduler

scheduler: scheduler.c graphics.c policies/fifo.c policies/sjf.c policies/rr.c policies/priorite.c
	$(CC) $(CFLAGS) -o scheduler scheduler.c graphics.c policies/fifo.c policies/sjf.c policies/rr.c policies/priorite.c

clean:
	rm -f scheduler

run: scheduler
	./scheduler processes.txt

.PHONY: all clean run
