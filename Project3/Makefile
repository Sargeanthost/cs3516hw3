CC = gcc

.PHONY: all clean

all: project3

project3: node0.c node1.c node2.c node3.c project3.c
	$(CC) -g project3.c node0.c node1.c node2.c node3.c -o project3

clean:
	rm -f project3