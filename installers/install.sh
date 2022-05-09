#!/bin/bash

sudo at-get update

sudo apt-get install mysql-server mysql-common mysql-client
sudo apt-get install g++
sudo apt-get install git

if [ "$1" = "-c" ]
then
git clone "https://github.com/anime-frog/work-with-mysql"
fi
