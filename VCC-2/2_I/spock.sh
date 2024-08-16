#! /bin/bash
make 
#loading the kernel module
sudo insmod q2a.ko
#Providing the required permission
sudo chmod 777 /dev/my_ioctl
#Compiling the test program
gcc -o test test.c
./test
sudo dmesg
sudo rmmod q2a
