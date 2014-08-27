CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=pch.cpp main.cpp ../common/serialport.cpp
INC=-I./ -I../common
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=hello

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(INC) $< -o $@