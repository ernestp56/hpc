/* Purpose:  Implement Dijkstra's algorithm for solving the single-source
 *           shortest path problem: find the length of the shortest path
 *           between a specified vertex and all other vertices in a directed
 *           graph.
 *
 * Input:    n, the number of vertices in the digraph
 *           mat, the adjacency matrix of the digraph
 * Output:   A list showing the cost of the shortest path
 *           from vertex 0 to every other vertex in the graph.
 *
 * Compile:  mpicc -g -Wall -o dijkstra.c dijkstra.c.c
 * Run:      mpiexec -n <p> ./dijkstra.c
 *           For large matrices, put the matrix into a file with n as
 *           the first line and run with ./dijkstra < large_matrix
 * CODE:     https://github.com/Lehmannhen/MPI-Dijkstra
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <sys/time.h>
#include <math.h>

//#ifndef MAX_BUF
#define MAX_BUF 200
//#endif

#define threads 5

#define MAX_STRING 10000
#define INFINITY_NEW 1000000

#define file_num "50"

struct partialTimes {
    float partial_time1, partial_time2, partial_time3, partial_time4;
};

typedef struct partialTimes Struct;

int Read_n(int my_rank, MPI_Comm comm);
MPI_Datatype Build_blk_col_type(int n, int loc_n);
void Read_matrix(float loc_mat[], int n, int loc_n,
                 MPI_Datatype blk_col_mpi_t, int my_rank, MPI_Comm comm);
void Print_local_matrix(float loc_mat[], int n, int loc_n, int my_rank);
void Print_matrix(float loc_mat[], int n, int loc_n,
                  MPI_Datatype blk_col_mpi_t, int my_rank, MPI_Comm comm);
Struct Dijkstra(float mat[], float dist[], int pred[], int n, int loc_n, int my_rank,
    MPI_Comm comm);
void Initialize_matrix(float mat[], float loc_dist[], int loc_pred[], int known[],
    int loc_n, int my_rank);
int Find_min_dist(float loc_dist[], int known[], int loc_n, int my_rank,
    MPI_Comm comm);
int Global_vertex(int loc_u, int loc_n, int my_rank);
void Print_dists(float loc_dist[], int n, int loc_n, int my_rank, MPI_Comm comm);
void Print_paths(int loc_pred[], int n, int loc_n, int my_rank, MPI_Comm comm);




int main(int argc, char* argv[]) {
    float *loc_mat, *loc_dist;
    int  *loc_pred;
    int n, loc_n, p, my_rank, run;
    MPI_Comm comm;
    MPI_Datatype blk_col_mpi_t;
    double time_start, time_end, time_start_matrix, time_end_matrix;
    double partial_time1 = 0, partial_time2 = 0, partial_time3 = 0, partial_time4 = 0;
    double total_time = 0, total_time_matrix = 0;
    struct timeval tv;
    struct timezone tz;
    Struct times;

    MPI_Init(&argc, &argv);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &my_rank);

    for (run=0; run<5; run++) {

        n = Read_n(my_rank, comm);
        loc_n = n/p;

        if (n%p != 0) {
            if (my_rank == 0) exit (0);
        }

        loc_mat = malloc(n*loc_n*sizeof(float));
        loc_dist = malloc(n*loc_n*sizeof(float));
        loc_pred = malloc(n*loc_n*sizeof(int));

        blk_col_mpi_t = Build_blk_col_type(n, loc_n);

        gettimeofday(&tv, &tz);
        time_start_matrix = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
        Read_matrix(loc_mat, n, loc_n, blk_col_mpi_t, my_rank, comm);
        gettimeofday(&tv, &tz);
        time_end_matrix = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
        total_time_matrix += time_end_matrix - time_start_matrix;

        gettimeofday(&tv, &tz);
        time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
        times = Dijkstra(loc_mat, loc_dist, loc_pred, n, loc_n, my_rank, comm);
        gettimeofday(&tv, &tz);
        time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
        total_time += time_end - time_start;
        partial_time1 += times.partial_time1;
        partial_time2 += times.partial_time2;
        partial_time3 += times.partial_time3;
        partial_time4 += times.partial_time4;

    }
    char path[MAX_BUF];
    getcwd(path, MAX_BUF);
    char buf[0x100];
    char file_name[0x100];
    snprintf(file_name,sizeof(file_name), "/hpc4ds/output/output_v4_%s.csv",file_num);
    snprintf(buf, sizeof(buf), "%s%s", path, file_name);

    if (my_rank == 0){
        printf("%-12d | %-12d | %-12d | %-12f | %-12f | %-12f | %-12f | %-12f | %-12f\n", p, n, loc_n, total_time/5,partial_time1/5,
        partial_time2/5,partial_time3/5,partial_time4/5, total_time_matrix/5);
        FILE* f = fopen(buf, "awb+");
        if (f==NULL) {
            printf("failed to open file: permission issue ?\n");
            exit(1);
        }
        else {
            fprintf(f,"%d %d %d %f %f %f %f %f %f\n", p, n, loc_n, total_time/5,partial_time1/5, partial_time2/5,partial_time3/5,partial_time4/5, total_time_matrix/5) ;
        }
        fclose(f);
    }

    //Print_dists(loc_dist, n, loc_n, my_rank, comm);
    //Print_paths(loc_pred, n, loc_n, my_rank, comm);

    free(loc_mat);
    free(loc_dist);
    free(loc_pred);

    MPI_Type_free(&blk_col_mpi_t);

    MPI_Finalize();

    return 0;
}  /* main */


/*---------------------------------------------------------------------
 * Function:  Read_n
 * Purpose:   Read in the number of rows in the matrix on process 0
 *            and broadcast this value to the other processes
 * In args:   my_rank:  the calling process' rank
 *            comm:  Communicator containing all calling processes
 * Ret val:   n:  the number of rows in the matrix
 */
int Read_n(int my_rank, MPI_Comm comm) {
    int n;

    if (my_rank == 0) {
        n = atoi(file_num);
    }

        /*
        char c;
        int word = 0;
        int count = 0;

        char path[MAX_BUF];
        getcwd(path, MAX_BUF);
        char buf[0x100];
        char file_name[0x100];
        snprintf(file_name, sizeof(file_name), "/hpc4ds/graph/graph%s.txt",file_num);
        snprintf(buf, sizeof(buf), "%s%s", path, file_name);
        //snprintf(buf, sizeof(buf), "%s/hpc4ds/graph/graph1000.txt", path);
        FILE *myFile = fopen(buf, "r");

        //read file into array
        if (myFile == NULL){
            printf("Error Reading File\n");
            exit (0);
        }
        while ((c = fgetc(myFile)) != EOF){
            if (c == ' ' || c == '\n') {
                word = 0;
            }
            else {
                if(!word){
                count++;
                word = 1;
                }
            }
        }
        n = sqrt(count);
    }
    */
    MPI_Bcast(&n, 1, MPI_INT, 0, comm);
    return n;
}  /* Read_n */


/*---------------------------------------------------------------------
 * Function:  Build_blk_col_type
 * Purpose:   Build an MPI_Datatype that represents a block column of
 *            a matrix
 * In args:   n:  number of rows in the matrix and the block column
 *            loc_n = n/p:  number cols in the block column
 * Ret val:   blk_col_mpi_t:  MPI_Datatype that represents a block
 *            column
 */
MPI_Datatype Build_blk_col_type(int n, int loc_n) {
    MPI_Aint lb, extent; // type of a variable able to contain a memory address
    MPI_Datatype block_mpi_t;
    MPI_Datatype first_bc_mpi_t;
    MPI_Datatype blk_col_mpi_t;

    MPI_Type_contiguous(
        /*count: number of elements in the new datatype*/ loc_n,
        /*oldtype: the MPI data type of each element*/    MPI_FLOAT,
        /*newtype: new datatype*/                         &block_mpi_t
        );

    //Gets the lower bound and extent for a datatype.
    MPI_Type_get_extent(
        /*datatype: datatype to get informatin on*/      block_mpi_t,
        /*Lower bound of datatype*/                      &lb,
        /*extent of datatype*/                           &extent);

    //Defines a new data type that consists of a specified number of blocks of a specified size.
    MPI_Type_vector(
        /*number of blocks in the created vector*/                                           n,
        /*blocklength: number of elements in each block*/                                    loc_n,
        /*stride: number of elements between the start of one block and the star of another*/n,
        /*oldtype: the datatype of each element*/                                            MPI_FLOAT,
        /*newtype*/                                                                          &first_bc_mpi_t);

    // creates a datatype with a new lower bound and extent from an existing datatype.
    MPI_Type_create_resized(
        /*oldtype: input datatype*/      first_bc_mpi_t,
        /*new lower bound of datatype*/  lb,
        /*new extent of datatype*/       extent,
        /*newtype: output datatype*/     &blk_col_mpi_t);

    //Commits the datatype. Input is datatype to commit
    MPI_Type_commit(&blk_col_mpi_t);

    // Frees the datatype.
    MPI_Type_free(
      /* datatype that is to be freed */ &block_mpi_t);
    MPI_Type_free(&first_bc_mpi_t);

    return blk_col_mpi_t;
}  /* Build_blk_col_type */


/*---------------------------------------------------------------------
 * Function:  Read_matrix
 * Purpose:   Read in an nxn matrix of ints on process 0, and
 *            distribute it among the processes so that each
 *            process gets a block column with n rows and n/p
 *            columns
 * In args:   n:  the number of rows in the matrix and the submatrices
 *            loc_n = n/p:  the number of columns in the submatrices
 *            blk_col_mpi_t:  the MPI_Datatype used on process 0
 *            my_rank:  the caller's rank in comm
 *            comm:  Communicator consisting of all the processes
 * Out arg:   loc_mat:  the calling process' submatrix (needs to be
 *               allocated by the caller)
 */
void Read_matrix(float loc_mat[], int n, int loc_n,
                 MPI_Datatype blk_col_mpi_t, int my_rank, MPI_Comm comm) {
    float* mat = NULL;
    int i, j;

    // Process 0 is the one dsitributing the information
    // Enter the data of the matrix

    // char wd[256];
    // printf("current working dir " + getcwd(wd, sizeof(wd)));
    if (my_rank == 0) {
        char path[MAX_BUF];
        getcwd(path, MAX_BUF);
        char buf[0x100];
        char file_name[0x100];
        snprintf(file_name,sizeof(file_name), "/hpc4ds/graph/graph%s.txt",file_num);
        snprintf(buf, sizeof(buf), "%s%s", path, file_name);
        //snprintf(buf, sizeof(buf), "%s/hpc4ds/graph/graph1000.txt", path);
        //mat = malloc(n*n*sizeof(int));

        FILE *myFile = fopen(buf, "r");

        //read file into array
        if (myFile == NULL){
            printf("Error Reading File\n");
            exit (0);
        }
        // We check for allocation erros
        if ((mat = malloc(n*n*sizeof(float))) == NULL) {
            fprintf(stderr, "Error in initial allocation\n");
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                fscanf(myFile, "%f", &mat[i*n + j]);
        fclose(myFile);

       //We print the matrix
       /*
       printf("The matrix is: \n");
       for (i = 0; i < n; i++)
            for (j = 0; j < n; j++) {
                if ((i*n+j) % n != 0) printf("%12.2f ", mat[i*n + j]);
                else{
                    printf("\n");
                    printf("%12.2f \t ", mat[i*n + j]);
                }
            }
        */
    }
    MPI_Scatter(
        /*The handle to a buffer that contains the data to be sent by the root process.*/    mat,
        /*The number of elements in the send buffer*/                                        1,
        /*The data type of each element in the buffer.*/                                     blk_col_mpi_t,
        /*The handle to a buffer that contains the data that is received on each process*/   loc_mat,
        /*The number of elements in the receive buffer*/                                     n*loc_n,
        /*The data type of the elements in the receive buffer.*/                             MPI_FLOAT,
        /*source*/                                                                           0,
        /*Communicator*/                                                                     comm);

    if (my_rank == 0) free(mat);
}  /* Read_matrix */


/*---------------------------------------------------------------------
 * Function:  Print_local_matrix
 * Purpose:   Store a process' submatrix as a string and print the
 *            string.  Printing as a string reduces the chance
 *            that another process' output will interrupt the output.
 *            from the calling process.
 * In args:   loc_mat:  the calling process' submatrix
 *            n:  the number of rows in the submatrix
 *            loc_n:  the number of cols in the submatrix
 *            my_rank:  the calling process' rank
 */
void Print_local_matrix(float loc_mat[], int n, int loc_n, int my_rank) {
    char temp[MAX_STRING];
    char *cp = temp;
    int i, j;
    char *infi = "i";

    sprintf(cp, "\nProc %d >\n", my_rank);
    cp = temp + strlen(temp);
    for (i = 0; i < n; i++) {
        for (j = 0; j < loc_n; j++) {
            if (loc_mat[i*loc_n + j] == INFINITY_NEW)
                sprintf(cp, "%8s", infi);
            else
                sprintf(cp, "%8.2f ", loc_mat[i*loc_n + j]);
            cp = temp + strlen(temp);
        }
        sprintf(cp, "\n");
        cp = temp + strlen(temp);
    }

    printf("%s\n", temp);
}  /* Print_local_matrix */


/*---------------------------------------------------------------------
 * Function:  Print_matrix
 * Purpose:   Print the matrix that's been distributed among the
 *            processes.
 * In args:   loc_mat:  the calling process' submatrix
 *            n:  number of rows in the matrix and the submatrices
 *            loc_n:  the number of cols in the submatrix
 *            blk_col_mpi_t:  MPI_Datatype used on process 0 to
 *               receive a process' submatrix
 *            my_rank:  the calling process' rank
 *            comm:  Communicator consisting of all the processes
 */
void Print_matrix(float loc_mat[], int n, int loc_n,
                  MPI_Datatype blk_col_mpi_t, int my_rank, MPI_Comm comm) {
    float* mat = NULL;
    int i, j;

    if (my_rank == 0) mat = malloc(n*n*sizeof(float));
    MPI_Gather(loc_mat, n*loc_n, MPI_INT,mat, 1, blk_col_mpi_t, 0, comm);
    if (my_rank == 0) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++)
                if (mat[i*n + j] == INFINITY_NEW)
                    printf(" i ");
                else
                    printf("%12f ", mat[i*n + j]);
            printf("\n");
        }

        free(mat);
    }
}  /* Print_matrix */


/*-------------------------------------------------------------------
 * Function:    Dijkstra
 * Purpose:     Apply Dijkstra's algorithm to the matrix mat
 * In args:     mat: sub matrix
 *              n:  the number of vertices
 *              loc_n: size of loc arrays
 *              my_rank: rank of process
 *              MPI_Comm = MPI Communicator
 * In/Out args: loc_dist: loc dist array
 *              loc_pred: loc pred array
 */
Struct Dijkstra(float mat[], float loc_dist[], int loc_pred[], int n, int loc_n,
    int my_rank, MPI_Comm comm) {
    Struct s;
    int i, u, *known;
    float new_dist;
    int loc_u, loc_v;
    double time_start1, time_end1,  time_start2, time_end2, time_start3, time_end3, time_start4, time_end4;
    double partial_time1 = 0, partial_time2 = 0, partial_time3 = 0, partial_time4 = 0;
    struct timeval tv;
    struct timezone tz;

    known = malloc(loc_n*sizeof(int));

    gettimeofday(&tv, &tz);
    time_start1 = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
    Initialize_matrix(mat, loc_dist, loc_pred, known, loc_n, my_rank);
    gettimeofday(&tv, &tz);
    time_end1 = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
    partial_time1 = time_end1 - time_start1;

    //Print_local_matrix(mat, n, loc_n, my_rank);


    for (i = 1; i < n; i++) {
        gettimeofday(&tv, &tz);
        time_start2 = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
        loc_u = Find_min_dist(loc_dist, known, loc_n, my_rank, comm);
        gettimeofday(&tv, &tz);
        time_end2 = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
        partial_time2 += time_end2 - time_start2;
        float my_min[2], glbl_min[2];
        float g_min_dist;

        if (loc_u < INFINITY_NEW) {
            my_min[0] = loc_dist[loc_u];
            my_min[1] = Global_vertex(loc_u, loc_n, my_rank);
        } else {
            my_min[0] = INFINITY_NEW;
            my_min[1] = INFINITY_NEW;
        }


        gettimeofday(&tv, &tz);
        time_start3 = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;

        MPI_Allreduce(my_min, glbl_min, 1, MPI_FLOAT_INT, MPI_MINLOC, comm);

        //printf("\n ITER %d -- RANK %d -- min_dist %f -- min_u %d", i,my_rank, my_min[0], my_min[1]);
        gettimeofday(&tv, &tz);
        time_end3 = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
        partial_time3 += time_end3 - time_start3;

        u = glbl_min[1];
        g_min_dist = glbl_min[0];

        if (u/loc_n == my_rank) {
            loc_u = u % loc_n;
            known[loc_u] = 1;
        }

        gettimeofday(&tv, &tz);
        time_start4 = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
        for (loc_v = 0; loc_v < loc_n; loc_v++) {
           if (!known[loc_v]) {
               if(mat[u*loc_n + loc_v] == INFINITY_NEW) {
                   new_dist = INFINITY_NEW;
               } else {
                    new_dist = g_min_dist + mat[u*loc_n + loc_v];
               }

               if (new_dist < loc_dist[loc_v]) {
                   loc_dist[loc_v] = new_dist;
                   loc_pred[loc_v] = u;
                }
            }
        }
        gettimeofday(&tv, &tz);
        time_end4 = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
        partial_time4 += time_end4 - time_start4;
    }
    s.partial_time1 = partial_time1;
    s.partial_time2 = partial_time2;
    s.partial_time3 = partial_time3;
    s.partial_time4 = partial_time4;

    free(known);
    return s;
}  /* Dijkstra */


/*--------------------------------------------------------------------
 * Function:    Initialize_matrix
 * Purpose:     Initialize loc_dist, known, and loc_pred arrays
 * In args:     mat: matrix
 *              loc_dist: loc distance array
 *              loc_pred: loc pred array
 *              known: known array
 */
void Initialize_matrix(float mat[], float loc_dist[], int loc_pred[], int known[],
    int loc_n, int my_rank) {
    int v;

    for (v = 0; v < loc_n; v++) {
        loc_dist[v] = mat[0*loc_n + v];
        loc_pred[v] = 0;
        known[v] = 0;
    }

    if (my_rank == 0) {
        known[0] = 1;
    }
} /* Initialize_matrix */


/*-------------------------------------------------------------------
 * Function:    Find_min_dist
 * Purpose:     Find the vertex u with minimum distance to 0
 *              (dist[u]) among the vertices whose distance
 *              to 0 is not known.
 * In args:     loc_dist:   loc distance array
 *              loc_known:  loc known array
 *              loc_n:      size of loc arrays
 * Out args:    local vertex
 */
int Find_min_dist(float loc_dist[], int loc_known[], int loc_n, int my_rank,
    MPI_Comm comm) {
    int loc_v, loc_u;
    float loc_min_dist = INFINITY_NEW;

    loc_u = INFINITY_NEW;
    for (loc_v = 0; loc_v < loc_n; loc_v++)
        if (!loc_known[loc_v])
            if (loc_dist[loc_v] < loc_min_dist) {
                loc_u = loc_v;
                loc_min_dist = loc_dist[loc_v];
            }

    return loc_u;
}  /* Find_min_dist */


/*-------------------------------------------------------------------
 * Function:    Global_vertex
 * Purpose:     Convert local vertex to global vertex
 * In args:     loc_u:     local vertex
 *              loc_n:     local number of vertices
 *              my_rank:   rank of process
 * Out args:    global_u:  global vertex
 */
int Global_vertex(int loc_u, int loc_n, int my_rank) {
    int global_u = loc_u + my_rank*loc_n;
    return global_u;
} /* Global_vertex */


/*-------------------------------------------------------------------
 * Function:    Print_dists
 * Purpose:     Print the length of the shortest path from 0 to each
 *              vertex
 * In args:     n:  the number of vertices
 *              dist:  distances from 0 to each vertex v:  dist[v]
 *                 is the length of the shortest path 0->v
 */
void Print_dists(float loc_dist[], int n, int loc_n, int my_rank, MPI_Comm comm) {
    int v;

   float* dist = NULL;

   if (my_rank == 0) {
      dist = malloc(n*sizeof(float));
   }

   MPI_Gather(loc_dist, loc_n, MPI_FLOAT, dist, loc_n, MPI_FLOAT, 0, comm);

   if (my_rank == 0) {
        printf("\nThe distance from 0 to each vertex is:\n");
        printf("  v    dist 0->v\n");
        printf("----   ---------\n");

        for (v = 1; v < n; v++)
        printf("%3d       %.4f\n", v, dist[v]);
        printf("\n");


       //printf("check if known is zero: %p \n", dist);

      free(dist);
   }
} /* Print_dists */


/*-------------------------------------------------------------------
 * Function:    Print_paths
 * Purpose:     Print the shortest path from 0 to each vertex
 * In args:     n:  the number of vertices
 *              pred:  list of predecessors:  pred[v] = u if
 *              u precedes v on the shortest path 0->v
 */
void Print_paths(int loc_pred[], int n, int loc_n, int my_rank, MPI_Comm comm) {
   int v, w, *path, count, i;

   int* pred = NULL;

   if (my_rank == 0) {
      pred = malloc(n*sizeof(int));
   }

   MPI_Gather(loc_pred, loc_n, MPI_INT, pred, loc_n, MPI_INT, 0, comm);

   if (my_rank == 0) {
        path =  malloc(n*sizeof(int));

        printf("\nThe shortest path from 0 to each vertex is:\n");
        printf("  v     Path 0->v\n");
        printf("----    ---------\n");
        for (v = 1; v < n; v++) {
            printf("%3d:    ", v);
            count = 0;
            w = v;
            while (w != 0) {
                path[count] = w;
                count++;
                w = pred[w];
            }
            printf("0 ");
            for (i = count-1; i >= 0; i--)
                printf("%d ", path[i]);
            printf("\n");
        }
      free(path);
      free(pred);
    }
}  /* Print_paths */