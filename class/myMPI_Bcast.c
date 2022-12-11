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
  
  int number = -1;
  // Process 0 is going to send something to all processes
  // We could send one by one, but we can simplify by using the
  // broadcast operator

  if (world_rank == 0) {
    // We read the data
    printf("Enter the value of the number\n");
    scanf("%d",&number);
  }
  
  MPI_Bcast(
      /* data         = */ &number, 
      /* count        = */ 1, 
      /* datatype     = */ MPI_INT, 
      /* source       = */ 0, 
      /* communicator = */ MPI_COMM_WORLD);
  if (world_rank == 0)  printf("Process %d sent the message \n", world_rank);
  else printf("Process %d received the message \n", world_rank);

  
  MPI_Finalize();
  return 0;
}
