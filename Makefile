.PHONY = clean test

all: build/libw2.so build/_w2.cpython-37m-x86_64-linux-gnu.so clean

PQ_DEP = c-generic-pqueue
TEST_DIR = tests
CFLAGS = -std=c99 -Wall -Werror

$(PQ_DEP)/pqueue.o: $(PQ_DEP)/pqueue.c
	gcc -g $(PQ_DEP)/pqueue.c -c -fPIC -o $(PQ_DEP)/pqueue.o

search: w2.o
	gcc -g -I. -Lbuild -lw2 main.c $(CFLAGS) -o $@

w2.o: w2.c
	gcc -g w2.c -c -fPIC $(CFLAGS)

build/libw2.so: w2.o $(PQ_DEP)/pqueue.o
	ar rc build/libw2.so w2.o $(PQ_DEP)/pqueue.o

build/_w2.cpython-37m-x86_64-linux-gnu.so: build/libw2.so
	python3 python-bindings/w2_cffi.py
	mv _w2_cffi.cpython-37m-x86_64-linux-gnu.so build/
	mv _w2_cffi.c build/
	mv _w2_cffi.o build/

$(TEST_DIR)/test: build/libw2.so
	gcc -g -I. -Lbuild -lw2 -std=c99 $(TEST_DIR)/test.c -o $(TEST_DIR)/test

test: $(TEST_DIR)/test clean
	$(TEST_DIR)/test

clean:
	rm $(PQ_DEP)/pqueue.o
	rm w2.o
	rm build/*.o

# gcc -g -I. -Lbuild -lw2 tests/test.c -o tests/test pqueue.o
