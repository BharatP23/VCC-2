#!/bin/bash

make -s
make -f Makefile_dot_c


sudo insmod q2b.ko
sudo chmod 777 /dev/parent_change

./control_station &
c_pid=$!
echo "Control station PID: $c_pid"
sleep 3

./soldier $c_pid &
echo "Soldier PID: $!"
sleep 3
kill -9 $c_pid

sudo rmmod q2b

make -s clean
make -s -f Makefile_dot_c clean