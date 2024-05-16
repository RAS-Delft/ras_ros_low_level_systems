#!/bin/bash

arduino_port=$(ls -l /dev/serial/by-id/ | grep Arduino | awk '{print $NF}' | awk -F '/' '{print $NF}')

echo "/dev/${arduino_port}"

