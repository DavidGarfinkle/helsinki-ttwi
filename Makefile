all: libw2.a

pqueue.o: w2/c_pqueue/pqueue.c
	gcc w2/c_pqueue/pqueue.c -c -fPIC

w2.o: w2/w2.c
	gcc w2/w2.c -c -fPIC

libw2.a: w2.o pqueue.o
	ar rc w2/libw2.a w2.o pqueue.o

test:
	make w
	w2/w2 "tests/query_a.vectors" "tests/leiermann.vectors" "tests/lemstrom.res"
