# configure

CC = g++
STD = -std=c++11
TARGET = main

# compile

main: main.o variable.o
	$(CC) main.o variable.o -o $(TARGET) $(STD)

variable.o: variable.cpp variable.h evaldef.h exception.h
	$(CC) -c variable.cpp $(STD)

main.o: main.cpp variable.h
	$(CC) -c main.cpp $(STD)

clean:
	rm *.o