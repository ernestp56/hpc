#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  // Find out rank, size
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // We are assuming at least 4 processes for this task
  if (world_size < 4) {
    fprintf(stderr, "World size must be greater than 3 for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int number;
  /* As long as we are not in the 0th process, we want to receive from the previous*/
  if (world_rank != 0) {
    MPI_Recv(
      /* data         = */ &number, 
      /* count        = */ 1, 
      /* datatype     = */ MPI_INT, 
      /* source       = */ world_rank - 1, 
      /* tag          = */ 0, 
      /* communicator = */ MPI_COMM_WORLD, 
      /* status       = */ MPI_STATUS_IGNORE);
    printf("Process %d received number %d from process %d\n",world_rank, number, world_rank -1);

    /* If we are in 0, we have to send the number to 1*/
  } else {
    number = -1;    
  }
    MPI_Send(
      /* data         = */ &number, 
      /* count        = */ 1, 
      /* datatype     = */ MPI_INT, 
      /* destination  = */ world_rank +1, 
      /* tag          = */ 0, 
      /* communicator = */ MPI_COMM_WORLD);

  /* Now we can make process 1 receive the process from 4*/
  if (world_rank == 0){
    MPI_Recv(
      /* data         = */ &number, 
      /* count        = */ 1, 
      /* datatype     = */ MPI_INT, 
      /* source       = */ world_rank - 1, 
      /* tag          = */ 0, 
      /* communicator = */ MPI_COMM_WORLD, 
      /* status       = */ MPI_STATUS_IGNORE);
    printf("Process %d received number %d from process %d\n",world_rank, number, world_size -1);
  }
  MPI_Finalize();
  return 0;
}
