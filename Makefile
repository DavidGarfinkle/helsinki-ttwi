w: 
	gcc w2/w2.c w2/c_pqueue/pqueue.c -g -o w2/w2

w-python:
	swig -python -c++ -o w2/_w2_module.cc w2/w2.i
	python setup.py build_ext --inplace

w-cffi:
	gcc w2/w2.c w2/c_pqueue/pqueue.c -c -fPIC
	ar rc w2/libw2.a w2.o pqueue.o
	python3 w2/w2_ffi.py
	make clean-w-cffi

clean-w-cffi:
	rm *.o

debug-mac:
	make w
	lldb --file w2/w2 --source tests/debug.lldb -- "tests/query_a.vectors" "tests/leiermann.vectors" "tests/lemstromm.res"

test:
	make w
	w2/w2 "tests/query_a.vectors" "tests/leiermann.vectors" "tests/lemstrom.res"
	cat tests/lemstrom.res
	rm w2/w2

testStream:
	make w
	cat "tests/data/lemstrom2011/query_a.mid.vectors" | patternfinder/geometric_helsinki/_w --stream "tests/data/lemstrom2011/leiermann.xml.vectors"

clean:	clean-w-cffi
