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

  int ping_pong_count = 0;
  int partner_rank = (world_rank + 1) % 2;
  const int LIMIT = 10;

  if (world_size != 2) {
    fprintf(stderr, "World size must be 2 for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  
   while (ping_pong_count < LIMIT) {
    if (world_rank == ping_pong_count % 2) { 
        ping_pong_count++; /* we increment the count*/
        MPI_Send(
      /* data         = */ &ping_pong_count, 
      /* count        = */ 1, 
      /* datatype     = */ MPI_INT, 
      /* destination  = */ partner_rank, 
      /* tag          = */ 0, 
      /* communicator = */ MPI_COMM_WORLD);
        printf("Process %d incremented count from %d to %d and sent it to process %d\n",world_rank, ping_pong_count-1, ping_pong_count,partner_rank);

    } else {
        MPI_Recv(
      /* data         = */ &ping_pong_count, 
      /* count        = */ 1, 
      /* datatype     = */ MPI_INT, 
      /* source       = */ partner_rank, 
      /* tag          = */ 0, 
      /* communicator = */ MPI_COMM_WORLD, 
      /* status       = */ MPI_STATUS_IGNORE);
        printf("Process %d received ping pong count %d from process %d\n",world_rank, ping_pong_count, partner_rank);
    }
   }
  MPI_Finalize();
  return 0;
}
