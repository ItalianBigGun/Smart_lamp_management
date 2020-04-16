#!/bin/bash  
g++ -c ./src/testdev.cpp -o ./lib/testdev.o -I ./include -lboost_system -lboost_thread  -std=c++11 -lpthread
