#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  // Initialize the MPI environment
  MPI_Init(&argc, &argv);
  // Find out rank, size
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  int number;
  do {
    if (world_rank == 0) {
        number = -1; 
        printf("Enter the number\n");
        scanf("%d", &number);
         MPI_Send(
        /* data         = */ &number, 
        /* count        = */ 1, 
        /* datatype     = */ MPI_INT, scanf("%d", &number);
        /* destination  = */ world_rank +1, 
        /* tag          = */ 0, 
        /* communicator = */ MPI_COMM_WORLD);

    }
    else { 
        MPI_Recv(
        /* data         = */ &number, 
        /* count        = */ 1, 
        /* datatype     = */ MPI_INT, 
        /* source       = */ world_rank - 1, 
        /* tag          = */ 0, 
        /* communicator = */ MPI_COMM_WORLD, 
        /* status       = */ MPI_STATUS_IGNORE);

        if (world_rank < world_size -1 ) {
            MPI_Send(
            /* data         = */ &number, 
            /* count        = */ 1, 
            /* datatype     = */ MPI_INT, 
            /* destination  = */ world_rank +1, 
            /* tag          = */ 0, 
            /* communicator = */ MPI_COMM_WORLD);

        }
    }    
    printf("Process %d received number %d from process %d\n",world_rank, number, world_rank -1);
  } while (number >= 0);
    /* If we are in 0, we have to send the number to 1*/
  MPI_Finalize();
  return 0;
}
