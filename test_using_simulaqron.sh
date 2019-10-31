#!/usr/bin/env bash

set -eu

# Install SimulaQron
if [ -z "$(which simulaqron)" ]; then
    echo "To run these tests you need simulaqron installed.\nYou can install simulaqron by 'pip3 install simulaqron'."
    exit 1
fi
# read -p "SimulaQron will be installed using pip, do you wish to continue? (y/n)" yn
# if [ "$yn" == "y" ]; then
#     pip3 install simulaqron
# else
#     exit
# fi

# Clean up leftovers
simulaqron stop
simulaqron reset -f

# Set the network file to be the one in this folder
this_file_path=$0
this_folder_path=$(dirname "$(readlink -f ${this_file_path})")
network_file="${this_folder_path}/test_network.json"
simulaqron set network-config-file $network_file
# simulaqron set log-level 0

# Start the Virtual and CQC Nodes
echo -e "\e[1;32m[$(date +%H:%M:%S)] Start CQC Nodes\e[0m"
simulaqron start --nodes="Alice,Bob" --keep
sleep 5

# Build the tests
echo -e "\e[1;32m[$(date +%H:%M:%S)] Build Tests\e[0m"
make all

# Start the tests
echo -e "\e[1;32m[$(date +%H:%M:%S)] Run Tests\e[0m"
./qubit localhost 8803
./send localhost 8803 localhost 8804
./recv localhost 8804
./gates localhost 8803

echo -e "\e[1;32m[$(date +%H:%M:%S)] Testing Complete\e[0m"
simulaqron stop
