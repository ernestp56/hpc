#include <mpi.h>
#include <stdio.h>
#include <string.h>

const int MAX_STRING = 100;

int main (void) {
    char greeting[MAX_STRING];
    int comm_sz; /* Number of porcesses*/
    int my_rank; /*My process rank*/

    /* MPI_Init defines a communicator that 
    consists of all the processes created when 
    the program is started*/
    MPI_Init(NULL, NULL);
    /* Communicators */
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank != 0) {
        sprintf(greeting, "Greetings from process %d of %d!", my_rank, comm_sz);
        /* message, size of message, datatype, destin, tag, communicator*/
        MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

    } else {
        printf("Greetings from process %d of %d! \n", my_rank, comm_sz);
        int q;
        for (q = 1; q < comm_sz; q++) {
            /* message, size of message, type of message, source, tag, communicator, status*/
            MPI_Recv(greeting, MAX_STRING, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s\n", greeting);
        }
    }
    MPI_Finalize();
    return 0;

}
