#!/bin/bash  
g++ -c ../src/Sql.cpp -o ../lib/Sql.o -I ../include -lboost_system -lboost_thread -std=c++11