#!/bin/bash

#number of threads:
for	 t	in 1 4
do
		echo ""
		echo NUMT = $t
		g++   -DNUMT=$t  proj00.cpp -o proj00 -lm -fopenmp
		./proj00
done
