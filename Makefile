CC=g++
CXXFLAGS=-std=c++11 -g
LDFLAGS=-g

all: thistlethwaite


thistlethwaite: thistlethwaite.o


clean:
	  rm -f thistlethwaite *.o core*
