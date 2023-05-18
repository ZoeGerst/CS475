#!/bin/bash
 for n in 1000 5000 16000 70000 300000 1000000 4000000 8000000
 do
     g++   all04.cpp  -DARRAYSIZE=$n  -o all04  -lm  -fopenmp
    ./all04
 done
