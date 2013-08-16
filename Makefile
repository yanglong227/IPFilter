OBJ=main.o filter.o
Filter: $(OBJ)
	g++ -g -o Filter main.o filter.o
main.o: main.cpp
	g++ -g -c main.cpp
filter.o: filter.h filter.cpp
	g++ -g -c filter.cpp
.PHONY: clean
clean:
	rm -rf *.o *.gch Filter
