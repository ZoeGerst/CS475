#!/bin/bash
#SBATCH -J Project7
#SBATCH -A cs475-575
#SBATCH -p classmpitest
#SBATCH -N 4 # number of nodes
#SBATCH -n 4 # number of tasks
#SBATCH --constraint=ib
#SBATCH -o proj07.out
#SBATCH -e proj07.err
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=gerstz@oregonstate.edu
module load openmpi
mpic++ proj07.cpp -o proj07 -lm
mpiexec -mca btl self,tcp -np 4 ./proj07
