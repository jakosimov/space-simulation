
all: main.o initialCondition.o
	g++ main.o initialCondition.o -lsfml-graphics -lsfml-window -lsfml-system -o output

main.o: main.cpp
	g++ -c main.cpp

initialCondition.o: initialCondition.cpp
	g++ -c initialCondition.cpp

run: all
	./output

clean:
	rm *.o *~ output 
