#!/bin/bash

#PBS -l select=1:ncpus=1:mem=2gb

#set max execution time
#PBS -l walltime=5:00:00

#PBS -q short_cpuQ
module load mpich-3.2

#read _ _ PBS_WALLTIME  <<< `qstat -f $PBS_JOBID | grep "Resource_List.walltime"`
#echo $PBS_WALLTIME

printf "%-12s | %-12s |  %-12s\n"  "Elements" "Threads" "time cost"

for i in $(seq 1 50); do
    mpirun.actual ./hpc4ds/src/openmp/dijkstra_openmp $i;
done

#mpicc -g -Wall -fopenmp −o src/openmp/dijkstra_openmp src/openmp/dijkstra_openmp.c -lm
