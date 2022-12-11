#!/bin/bash

#PBS -l select=1:ncpus=1:mem=2gb

#set max execution time
#PBS -l walltime=0:05:00

#
#PBS -q short_cpuQ
module load mpich-3.2
#mpirun.actual -n 4 ./hpc4ds/mpi-hello-world
#mpirun.actual -n 4 ./hpc4ds/trapezoid
#mpirun.actual -n 4 ./hpc4ds/ring
#mpirun.actual -n 2 ./hpc4ds/ping_pong
#mpirun.actual -n 5 ./hpc4ds/ring_v2
#mpirun.actual -n 7 ./hpc4ds/myMPI_Bcast
mpirun.actual -n 2 ./hpc4ds/exercise


