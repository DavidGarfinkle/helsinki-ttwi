w: 
	git submodule update --init --recursive
	gcc w2/algorithm-w2.c c-generic-pqueue/pqueue.c -I -g -o w2/algorithm-w2

w-python:
	swig -python -o w2/_w2_module.cc w2/algorithm-w2.i
	python setup.py build_ext --inplace

testgdb:
	make w
	gdb --args smr_search/_w "/home/dgarfinkle/patternfinder/tests/data/lemstrom2011/query_a.mid.vectors" "/home/dgarfinkle/patternfinder/tests/data/lemstrom2011/leiermann.xml.vectors" "/home/dgarfinkle/patternfinder/c_test/lemstromm.res"

test:
	make w
	patternfinder/geometric_helsinki/_w "tests/data/lemstrom2011/query_a.mid.vectors" "tests/data/lemstrom2011/leiermann.xml.vectors" "c_test/lemstrom.res"

testStream:
	make w
	cat "tests/data/lemstrom2011/query_a.mid.vectors" | patternfinder/geometric_helsinki/_w --stream "tests/data/lemstrom2011/leiermann.xml.vectors"

clean:
	rm *.o *.a
	rm algorithm-w2
