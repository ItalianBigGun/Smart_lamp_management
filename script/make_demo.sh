#!/bin/bash  
g++ ../lib/*.o -o demo -I ../include -lboost_system -lboost_thread -lmysqlclient -lprotobuf -lpthread -lboost_coroutine
