all: main board_puissance4.o list.o

list.o: list.c list.h
	gcc -c list.c

board_puissance4.o: board_puissance4.c board_puissance4.h
	gcc -c board_puissance4.c

main: puissance4.cpp board_puissance4.o list.o
	g++ -o main puissance4.cpp board_puissance4.o list.o -lsfml-graphics -lsfml-window -lsfml-system

clean: 
	rm -f main *.o