./build/program : main.o stringmap.o
	g++ -o $@ $^ -Wall
main.o : main.cpp
	g++ -c $^ -Wall
stringmap.o : stringmap.cpp
	g++ -c $^ -Wall
clean :
	rm *.o *.gch