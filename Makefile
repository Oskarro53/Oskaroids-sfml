CXX = g++

all: gra

run:
	./gra

gra: main.cpp
	$(CXX) main.cpp -o gra -lsfml-graphics -lsfml-window -lsfml-system

clean:
	rm -f gra *.o

.PHONY: all, clean, run