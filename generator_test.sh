#!/bin/bash
clear
for ((i=10; i<100; i++))
do
    for ((j=10; j<100; j++))
    do
        ./generator_test $i $j
    done
done
