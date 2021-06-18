#!/bin/bash
g++ ./src/*.cpp -I ./include -o demo -std=c++11 -lboost_thread -lboost_system -lmysqlclient
