#!/bin/bash

#PBS -l select=1:ncpus=1:mem=2gb

#set max execution time
#PBS -l walltime=5:00:00

#PBS -q short_cpuQ
module load mpich-3.2

#read _ _ PBS_WALLTIME  <<< `qstat -f $PBS_JOBID | grep "Resource_List.walltime"`
#echo $PBS_WALLTIME

printf "%-12s | %-12s | %-12s | %-12s | %-12s | %-12s | %-12s | %-12s | %-12s\n"  "Processors" "Nodes" "loc_n" "time cost" "time1" "time2" "time3" "time4" "time matrix"
> output_v4_50.csv
for i in $(seq 1 50); do
    mpirun.actual -n $i ./hpc4ds/src/mpi/djikstra;
done

#mpirun.actual ./hpc4ds/djikstra_serial

# When using the math library use -lm to compile
#mpicc -g -Wall -o djikstra djikstra.c -lm

