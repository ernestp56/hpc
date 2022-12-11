#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv) {
  // Initialize the MPI environment
  MPI_Init(&argc, &argv);
  // Find out rank, size
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size; // We need 2 processes
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  int i;  
  double elapsed_time[20];  // array of 20 elements

// Check we have 2 processes
  if (world_size != 2) {
    fprintf(stderr, "World size must be 2 for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  printf("Kind\t\tn\t\tbytes\t\ttime (m sec)\n");
    
// Loop over the message size from 1 Byte to 1MB  
   for (i = 0; i <= 20; i++) {

        long int N = 1 << i;
        char *ptr = (char*)malloc(N);

        // Check if the memory has been successfully allocated by malloc or not
        if (ptr == NULL) {
            printf("Memory not allocated.\n");
            exit(0);
        }

        int count = 0;
        const int LIMIT = 10;
        double t1,t2,sum = 0.0 ;
        
        
        while (count < LIMIT){
            if (world_rank == 0) {
                //printf("------- COUNT %d ------  \n", count);
                count++; 
                t1 = MPI_Wtime();
                //printf("t1 is %lf\n",t1);

                // Use MPI_WTime: Returns an elapsed time on the calling processor 
                // We set the time and we send the message

                MPI_Send(ptr,1,MPI_CHAR,1,0,MPI_COMM_WORLD);
                MPI_Recv(ptr,1,MPI_CHAR, 1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

                t2 = MPI_Wtime();
                //printf("t2 is %lf \n", t2);
                sum += (double)(t2-t1)/(2.0);
                
            } else if(world_rank == 1) {
                MPI_Recv(ptr,1,MPI_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                MPI_Send(ptr,1,MPI_CHAR,0,0,MPI_COMM_WORLD);
            }
        }

        if (world_rank == 0) {
            elapsed_time[i] = (double)(sum/LIMIT) *1000;
            printf("Send/Recv\t%d\t\t%ld\t\t\t%lf\n", i,N, elapsed_time[i]);
            free(ptr);
        }
   }
   
  // Estimate for message size

  // Allocate the arrays for different messages using malloc. Use dynamic arrays

  // Learn more about PBS placing strategies

  // Use MPI_Sendrecv. At the very end
  MPI_Finalize();
  return 0;
}

/* Sending a 0 message measures the latency, time/memory to open the channel. We are opening the channel but we are not 
sending anything. There is no contribution to message delivery.

If we do a linear interpolation over the message size series we can get the two parameter of the bandwith: latency and the other 
that is prportional to the message sie.
*/


