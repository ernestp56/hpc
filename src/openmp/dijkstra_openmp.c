/* The program that does OpenMP parallel Dijkstra’s Algorithm.*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

#define SOURCE 0
#define INFINITY_NEW 1000000
#define file_num "50"
#define MAX_BUF 200

int Read_n();
void Read_matrix(float *mat, int n);
void dijkstra (float mat[],int n, int source, float distance[], int pred[], int visited[]);

int main(int argc, char *argv[]) {
    float *mat;
    int n = Read_n();
    int threads = atoi(argv[1]);

    if (n%threads != 0) {
        exit (0);
    }

    float *distance;
    int *visited, *pred;
    mat = malloc(n*n*sizeof(float));
    /* This array holds the shortest distance from source to other vertices. */
    distance = malloc(n*sizeof(float));
    /* This value sets to 1 if vertices are finished to compute. */
    visited = malloc(n*sizeof(int));
    /* Array of predecesors. */
    pred = malloc(n*sizeof(int));

    omp_set_num_threads(threads);

    double time_start, time_end;
    struct timeval tv;
    struct timezone tz;

    Read_matrix(mat, n);

    //Matrix
    /*
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            fscanf(myFile, "%f", &mat[i*n + j]);
        }
    }

    printf("The matrix is: \n");
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++) {
            if ((i*n+j) % n != 0) printf("%12.2f ", mat[i*n + j]);
            else{
                printf("\n");
                printf("%12.2f ", mat[i*n + j]);
            }
        }
    fclose(myFile);
    */

    gettimeofday(&tv, &tz);
    time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
    dijkstra(mat, n, SOURCE, distance, pred, visited);
    gettimeofday(&tv, &tz);
    time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;

    char path[MAX_BUF];
    getcwd(path, MAX_BUF);
    char buf[0x100];
    char file_name[0x100];
    snprintf(file_name,sizeof(file_name), "/hpc4ds/output/output_openmp%s.csv",file_num);
    snprintf(buf, sizeof(buf), "%s%s", path, file_name);

    printf("%-12d | %-12d | %-12f\n", n, threads,  time_end - time_start);
    FILE* f = fopen(buf, "awb+");
    if (f==NULL) {
        printf("failed to open file: permission issue ?\n");
        exit(1);
    }
    else {
        fprintf(f,"%d %d %f\n", n, threads, time_end - time_start);
    }
    fclose(f);



    return 0;
}



void Read_matrix(float *mat, int n) {
    //float* mat = NULL;
    int i, j;

    char path[MAX_BUF];
    getcwd(path, MAX_BUF);
    char buf[0x100];
    char file_name[0x100];
    snprintf(file_name,sizeof(file_name), "/hpc4ds/graph/graph%s.txt",file_num);
    snprintf(buf, sizeof(buf), "%s%s", path, file_name);

    FILE *myFile = fopen(buf, "r");

    //read file into array
    if (myFile == NULL){
        printf("Error Reading File\n");
        exit (0);
    }
    // We check for allocation erros
    /*
    if ((mat = malloc(n*n*sizeof(float))) == NULL) {
        fprintf(stderr, "Error in initial allocation\n");
        exit(EXIT_FAILURE);
    }
    */
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            fscanf(myFile, "%f", &mat[i*n + j]);
    fclose(myFile);
    //free(mat);
}  /* Read_matrix */



void dijkstra(float graph[],int n, int source, float distance[], int pred[], int visited[]){
    int i, md, mv;
    int my_first; /* The first vertex that stores in one thread locally. */
    int my_id; /* ID for threads */
    int my_last; /* The last vertex that stores in one thread locally. */
    int my_md; /* local minimum distance */
    int my_mv; /* local minimum vertex */
    int my_step; /* local vertex that is at the minimum distance from the source */
    int nth; /* number of threads */

    /* Initialize all vertices' distance and status. */
    for (i = 0; i < n; i++) {
        distance[i] = graph[source*n + i];
        visited[i] = 0;
        pred[i] = 0;
    }
    visited[source] = 1;
    //count = 1;

    /* OpenMP parallelization starts here */
    #pragma omp parallel private ( my_first, my_id, my_last, my_md, my_mv, my_step ) shared ( visited, md, distance, mv, nth, graph )
        {

        my_id = omp_get_thread_num ( );
        nth = omp_get_num_threads ( );
        my_first = (my_id * n ) / nth;
        my_last = ((my_id + 1) * n) / nth - 1;
        //fprintf(stdout, "P%d: First=%d Last=%d\n", my_id, my_first, my_last);

        for (my_step = 1; my_step < n; my_step++) {
            #pragma omp single
            {
                md = INFINITY_NEW;
                mv = -1;
            }
            int k;
            my_md = INFINITY_NEW;
            my_mv = -1;

            /* Each thread finds the minimum distance unconnected vertex inner of
            the graph */
            for (k = my_first; k <= my_last; k++) {
                if (!visited[k] && distance[k] < my_md) {
                    my_md = distance[k];
                    my_mv = k;
                }
            }
            /* 'critical' specifies that code is only be executed on one thread at a time,
            * because we need to determine the minimum of all the my_md here. */
            #pragma omp critical
            {
                if (my_md < md) {
                    md = my_md;
                    mv = my_mv;
                }
            }
            /* 'barrier' identifies a synchronization point at which threads in a parallel
            * region will wait until all other threads in this section reach the same point. So
            * that md and mv have the correct value. */
            #pragma omp barrier
            #pragma omp single
            {
                /* It means we find the vertex and set its status to true. */
                if (mv != - 1){
                    visited[mv] = 1;
                }
            }

            #pragma omp barrier
                if ( mv != -1 ){
                    int j;
                    for (j = my_first; j <= my_last; j++) {
                         if (!visited[j] && graph[mv*n + j] < INFINITY_NEW &&
                            distance[mv] + graph[mv*n + j] < distance[j]) {
                            distance[j] = distance[mv] + graph[mv*n + j];
                        }
                    }
                }
            #pragma omp barrier
        }
    }
    /*
    for (i = 0; i < n; i++) {
        printf("%f ", distance[i]);
    }
    */
}


int Read_n() {
    int n;
    n = atoi(file_num);

    return n;
}  /* Read_n */