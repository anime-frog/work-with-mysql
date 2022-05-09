#!/bin/bash

sudo apt-get update

sudo apt-get purge mysql-server mysql-common mysql-client
sudo apt-get purge g++

rm -r ../work_with_mysql
