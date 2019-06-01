.PHONY = clean

all: libw2.a clean

PQ_DEP = c-generic-pqueue

$(PQ_DEP)/pqueue.o: $(PQ_DEP)/pqueue.c
	gcc -O3 $(PQ_DEP)/pqueue.c -c -fPIC -o $(PQ_DEP)/pqueue.o -std=c99

w2.o: w2.c
	gcc -O3 w2.c -c -fPIC -std=c99

libw2.a: w2.o $(PQ_DEP)/pqueue.o
	ar rc libw2.a w2.o $(PQ_DEP)/pqueue.o

clean:
	rm $(PQ_DEP)/pqueue.o
	rm w2.o
