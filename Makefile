
all: main.o
	g++ main.o -lsfml-graphics -lsfml-window -lsfml-system -o output

main.o: main.cpp
	g++ -c main.cpp


run: all
	./output

clean:
	rm *.o *~ output 
