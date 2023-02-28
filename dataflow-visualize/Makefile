default: parse

parse: parse.cpp
	mkdir -p build
	zapcc++ -O2 -march=native -mtune=native -lgumbo parse.cpp -o build/select
