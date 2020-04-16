#!/bin/bash
g++ ./lib/*.o -o demodev -I ./include -lboost_system -lboost_thread -lmysqlclient -lprotobuf -lpthread
