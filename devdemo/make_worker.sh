#!/bin/bash  
g++ -c ./src/worker.cpp -o ./lib/worker.o -I ./include -lboost_system -lboost_thread  -std=c++11