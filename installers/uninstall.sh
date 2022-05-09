#!/bin/bash

#sudo apt-get update

if [ "$1" = "-a" ]
then
sudo apt-get purge mysql-server mysql-common mysql-client
sudo apt-get purge g++
fi

if [[ "$1" = "-t" || "$2" = "-t" ]]
then
rm -rf `pwd`
else
if [ -e "$1" ]
then
rm -rf "$1" "$0"
elif [ -e "$2" ]
then
rm -rf "$2" "$0"
fi
fi