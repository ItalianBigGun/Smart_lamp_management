#!/bin/bash  
g++ -c ../src/filter.cpp -o ../lib/filter.o -I ../include -lboost_system -lboost_thread  -std=c++11