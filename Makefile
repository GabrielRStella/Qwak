.PHONY: clean test

test:
	g++ -std=c++11 ginac_test.cpp -lcln -lginac -o test
	./test