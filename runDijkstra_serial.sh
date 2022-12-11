#!/bin/bash

#PBS -l select=1:ncpus=1:mem=2gb

#set max execution time
#PBS -l walltime=5:00:00

#PBS -q short_cpuQ
module load mpich-3.2

#read _ _ PBS_WALLTIME  <<< `qstat -f $PBS_JOBID | grep "Resource_List.walltime"`
#echo $PBS_WALLTIME

printf "%-12s  |  %-12s\n"  "Elements" "time cost"
mpirun.actual ./hpc4ds/src/serial/dijkstra_serial

# When using the math library use -lm to compile
#mpicc -g -Wall -o src/serial/dijkstra_serial src/serial/dijkstra_serial.c -lm
