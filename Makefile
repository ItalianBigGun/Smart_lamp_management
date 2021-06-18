
CC := gcc
CXX := g++

LIBS := -lboost_system -lboost_thread -lprotobuf -lboost_coroutine -lmysqlclient -std=c++11 -lpthread 
SRC_PATH := src
INC_PATH := include
CPPFLAGS := -I ./include -std=c++11

OBJS := $(SRC_PATH)/Sql.o	\
	$(SRC_PATH)/filter.o	\
	$(SRC_PATH)/socketqueue.o	\
	$(SRC_PATH)/taskqueue.o	\
	$(SRC_PATH)/test.o	\
	$(SRC_PATH)/test_pb.o	\
	$(SRC_PATH)/worker.o

INCLUDES := $(INC_PATH)/Sql.h	\
	$(INC_PATH)/filter.h	\
	$(INC_PATH)/socketqueue.h	\
	$(INC_PATH)/taskqueue.h	\
	$(INC_PATH)/test.pb.h	\
	$(INC_PATH)/worker.h 

TARGET := smartSystem

$(TARGET): $(OBJS)
	$(CXX) -g -o $@ $^ $(LIBS)

clean:
	-rm $(OBJS) $(TARGET)
