#!/bin/bash
PATH=.
SRC_PATH=$(PATH)/src
INCLUDE_PATH=$(PATH)/include
LIB_PATH=$(PATH)/lib
OBJECT=$(LIB_PATH)/filter.o $(LIB_PATH)/Sql.o $(LIB_PATH)/test.o $(LIB_PATH)/socketqueue.o $(LIB_PATH)/taskqueue.o

CC=gcc

filter.o:$(SRC_PATH)/filter.cpp $(INCLUDE_PATH)/filter.h
	$(CC) -c $(SRC_PATH)/filter.cpp

Sql.o:$(SRC_PATH)/Sql.cpp $(INCLUDE_PATH)/Sql.h
	$(CC) -c $(SRC_PATH)/Sql.cpp

test.o:$(SRC_PATH)/test.cpp $(INCLUDE_PATH)/worker.h
	$(CC) -c $(SRC_PATH)/test.cpp

socketqueue.o:$(SRC_PATH)/socketqueue.cpp $(INCLUDE_PATH)/socketqueue.h
	$(CC) -c $(SRC_PATH)/socketqueue.cpp

taskqueue.o:$(SRC_PATH)/taskqueue.cpp $(INCLUDE_PATH)/taskqueue.h
	$(CC) -c $(SRC_PATH)/taskqueue.cpp

demo:$(OBJECT)
	$(CC) -o demo $(OBJECT)

clean:
	rm demo $(OBJECT)


