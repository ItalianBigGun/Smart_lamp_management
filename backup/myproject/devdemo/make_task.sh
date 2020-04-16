#!/bin/bash  
g++ -c ./src/taskqueue.cpp -o ./lib/taskqueue.o -I ./include -lboost_system -lboost_thread  -std=c++11
