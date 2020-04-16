#!/bin/bash  
g++ -c ./src/test.cpp -o ./lib/test.o -I ./include -lboost_system -lboost_thread  -std=c++11 -lpthread
