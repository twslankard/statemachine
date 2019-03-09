
CXXFLAGS=-Wpedantic -Wall -Werror -std=c++11 -pthread -I.

all:
	g++ ${CXXFLAGS} state_example.cpp -o state_example
	g++ ${CXXFLAGS} state_abort_example.cpp -o state_abort_example
