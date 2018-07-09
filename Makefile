cc = g++
prog = code

($prog) : mips.cpp
	g++ -o code mips.cpp -O2 -std=c++14 -march=corei7-avx

clean:
	rm -rf code
