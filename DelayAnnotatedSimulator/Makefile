CC=g++
CFLAGS=-c -Wall -std=c++11
OPTIMIZE2 = -O3
OPTIMIZE1 = -O

.PHONY : clang all

clang:CC=clang++

all:CFLAGS += ${OPTIMIZE2}
clang:CFLAGS += ${OPTIMIZE2}
TARGET=../build/fsim
OBJECTS= ../build/args.o ../build/circuit.o ../build/eventwheel.o ../build/gates.o ../build/inputvector.o ../build/main.o ../build/simulator.o ../build/fault.o

all: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) -std=c++11

clang: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) -std=c++11

../build/main.o: main.cpp Circuit.h Args.h Gates.h Simulator.h InputVector.h Type.h
	$(CC) $(CFLAGS) -o ../build/main.o main.cpp

../build/args.o: Args.cpp Args.h
	$(CC) $(CFLAGS) -o ../build/args.o Args.cpp

../build/circuit.o: Circuit.cpp Circuit.h Gates.h Type.h Fault.h
	$(CC) $(CFLAGS) -o ../build/circuit.o Circuit.cpp

../build/eventwheel.o: EventWheel.cpp EventWheel.h Gates.h Type.h
	$(CC) $(CFLAGS) -o ../build/eventwheel.o EventWheel.cpp

../build/inputvector.o: InputVector.cpp InputVector.h
	$(CC) $(CFLAGS) -o ../build/inputvector.o InputVector.cpp

../build/simulator.o: Simulator.cpp Simulator.h EventWheel.h Circuit.h Gates.h Args.h Type.h 
	$(CC) $(CFLAGS) -o ../build/simulator.o Simulator.cpp

../build/gates.o: Gates.cpp Gates.h Type.h Fault.h
	$(CC) $(CFLAGS) -o ../build/gates.o Gates.cpp

../build/fault.o: Gates.h Type.h Fault.h
	$(CC) $(CFLAGS) -o ../build/fault.o Fault.cpp

