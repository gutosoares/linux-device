#!/bin/bash
sudo rmmod device
make clean
make
sudo insmod device.ko
sudo mknod /dev/device c 60 0
sudo chmod 666 /dev/device
sudo chmod 666 /proc/device
echo SUCESSO
