./build/program : main.o block.o
	g++ -o $@ $^ -Wall
main.o : main.cpp
	g++ -c $^ -Wall
block.o : block.cpp
	g++ -c $^ -Wall
clean :
	rm *.o *.gch