wpp:
	gcc patternfinder/geometric_helsinki/_w.cpp patternfinder/geometric_helsinki/nlohmann/json.hpp -lstdc++ -std=c++11 -g -I patternfinder/geometric_helsinki/ -o patternfinder/geometric_helsinki/_wcpp -pg

testgdb:
	make w
	gdb --args smr_search/_w "/home/dgarfinkle/patternfinder/tests/data/lemstrom2011/query_a.mid.vectors" "/home/dgarfinkle/patternfinder/tests/data/lemstrom2011/leiermann.xml.vectors" "/home/dgarfinkle/patternfinder/c_test/lemstromm.res"

test:
	make w
	patternfinder/geometric_helsinki/_w "tests/data/lemstrom2011/query_a.mid.vectors" "tests/data/lemstrom2011/leiermann.xml.vectors" "c_test/lemstrom.res"

testStream:
	make w
	cat "tests/data/lemstrom2011/query_a.mid.vectors" | patternfinder/geometric_helsinki/_w --stream "tests/data/lemstrom2011/leiermann.xml.vectors"
