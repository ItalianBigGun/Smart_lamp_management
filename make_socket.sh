#!/bin/bash  
g++ -c ./src/socketqueue.cpp -o ./lib/socketqueue.o -I ./include -lboost_system -lboost_thread  -std=c++11
