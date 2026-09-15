CXX = g++

all: gra

run:
	./gra

gra: main.o Game.o
	$(CXX) main.o Game.o -o gra -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp Game.hpp
	$(CXX) -c main.cpp -o main.o

Game.o: Game.cpp Game.hpp
	$(CXX) -c Game.cpp -o Game.o

clean:
	rm -f gra *.o

.PHONY: all, clean, run