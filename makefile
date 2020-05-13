all: main

main: main.o map_reduce.o iface.o word_counter.o utils.o
	g++ -pthread main.o map_reduce.o word_counter.o utils.o -o main

main.o: main.cpp
	g++ -c main.cpp

map_reduce.o: src/map_reduce.cpp
	g++ -c src/map_reduce.cpp

iface.o: src/iface.cpp
	g++ -c src/iface.cpp

word_counter.o: src/examples/word_counter.cpp
	g++ -c examples/word_counter.cpp

utils.o: src/utils.cpp
	g++ -c src/utils.cpp

clean:
	rm -rf *.o main
