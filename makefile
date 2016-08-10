# configure

CC = g++
STD = -std=c++11
TARGET = main

# compile

main: main.o variable.o parser.o evaluator.o
	$(CC) main.o variable.o parser.o evaluator.o -o $(TARGET) $(STD)

variable.o: variable.cpp variable.h evaldef.h exception.h
	$(CC) -c variable.cpp $(STD)

parser.o: parser.cpp parser.h variable.h exception.h evaldef.h
	$(CC) -c parser.cpp $(STD)

evaluator.o: evaluator.cpp evaluator.h variable.h exception.h evaldef.h
	$(CC) -c evaluator.cpp $(STD)

main.o: main.cpp variable.h evaldef.h parser.h
	$(CC) -c main.cpp $(STD)

clean:
	rm *.o
	rm *.exe