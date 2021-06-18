#!/bin/bash 

./make_mysql.sh
echo "make_mysql success!"
./make_filter.sh
echo "make_filter success!"
./make_proto.sh
echo "make_proto success!"
./make_task.sh
echo "make_task success!"
./make_socket.sh
echo "make_worker success!"
./make_worker.sh
echo "make_socket success!"
./make_test.sh
echo "make_test success!"
./make_demo.sh
echo "make_all success!"