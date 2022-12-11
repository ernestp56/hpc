/* seqDijk.c
* Test program that does sequential Dijkstra’s Algorithm.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
/* Define the source vertex. */
#define SOURCE 0
#define MAXINT 9999999
#define INFINITY_NEW 1000000


//#ifndef MAX_BUF
#define MAX_BUF 200
//#endif

#define file_num "10000"

struct partialTimes {
    float partial_time1, partial_time2, partial_time3, partial_time4;
};

/* Function that implements Dijkstra's single source shortest path algorithm
 for a graph represented by adjacency matrix, and use source vertex as input. */
int Read_n();
void dijkstra (float graph[],int n, int source, float distance[], int pred[], int visited[]);
void Print_dists(float distance[], int n);
void Print_paths(int ped[], int n);


int main (int argc, char *argv[]) {
    float *mat;
    int i, j, n;
    double time_start, time_end;
    struct timeval tv;
    struct timezone tz;

    n = atoi(file_num);;

    float *distance;
    int *visited, *pred;
    mat = malloc(n*n*sizeof(float));
    /* This array holds the shortest distance from source to other vertices. */
    distance = malloc(n*sizeof(float));
    /* This value sets to 1 if vertices are finished to compute. */
    visited = malloc(n*sizeof(int));
    /* Array of predecesors. */
    pred = malloc(n*sizeof(int));
    
    /* Open input file, read adjacency matrix */
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
    for (i = 0; i < n; i++) 
        for (j = 0; j < n; j++) 
            fscanf(myFile, "%f", &mat[i*n + j]);
    
    /*
    printf("The matrix is: \n");
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++) {
            if ((i*n+j) % n != 0) printf("%12.2f ", mat[i*n + j]);
            else{
                printf("\n");
                printf("%12.2f ", mat[i*n + j]);
            }
        }
    */
    fclose(myFile);

    gettimeofday(&tv, &tz);
    time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
    dijkstra(mat, n, SOURCE, distance, pred, visited);
    gettimeofday(&tv, &tz);
    time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;
    
    printf("%-12d  |  %-12f\n", n, time_end - time_start);
    
    //Print_dists(distance,n);
    //Print_paths(pred,n);

    free(distance);
    free(pred);
    free(visited);
    return 0;
}


/*-------------------------------------------------------------------
 * Function:    Dijkstra
 * Purpose:     Apply Dijkstra's algorithm to the matrix mat
 * In args:     mat: matrix
 * In/Out args: distance: dist array
 *              pred: pred array
 *              visited: array of visisted nodes
 */
void dijkstra(float graph[], int n, int source, float distance[], int pred[], int visited[]) {
    
    int i, count, nextNode;
    float minDistance; 
    /* Initialize all vertices' distance and status. */
    for (i = 0; i < n; i++) {
        distance[i] = graph[source*n + i];
        visited[i] = 0;
        pred[i] = 0;
    }
    visited[source] = 1;
    count = 1;
    /* Find shortest path for all vertices. */
    while (count < n) {
        minDistance = INFINITY_NEW;
        /* Pick the minimum distance vertex from the set of vertices that
        is not processed. */
        for (i = 0; i < n; i++) {
            if (distance[i] < minDistance && !visited[i]) {
                minDistance = distance[i];
                nextNode = i; 
            }           
        }
        /* Mark this vertex is true. That means the vertex is processed. */
        visited[nextNode] = 1;
        count++;
        /* Update the dist value of the picked vertex. */
        for (i = 0; i < n; i++) {
            if (!visited[i] && minDistance + graph[nextNode*n + i] < distance[i]) {
                distance[i] = minDistance + graph[nextNode*n + i];
                pred[i] = nextNode;
            }
        }
    }
}


/*-------------------------------------------------------------------
 * Function:    Print_dists
 * Purpose:     Print the length of the shortest path from 0 to each
 *              vertex
 * In args:     distance:  distances from 0 to each vertex v:  dist[v]
 *                 is the length of the shortest path 0->v
 */
void Print_dists(float distance[],int n) {
    int i;
    /* Print the distance values. */
    printf("The distance from 0 to each vertex is:\n");
    printf("  v    dist 0->v\n");
    printf("----   ---------\n");
                     
    for (i = 1; i < n; i++) 
        printf("%3d       %.2f\n", i, distance[i]);
    printf("\n");
}


/*-------------------------------------------------------------------
 * Function:    Print_paths
 * Purpose:     Print the shortest path from 0 to each vertex
 * In args:     pred:  list of predecessors:  pred[v] = u if
 *              u precedes v on the shortest path 0->v
 */
void Print_paths(int pred[], int n) {
    int v, *path, count, w, i;
    /*Print the shortest path*/
    path =  malloc(n*sizeof(int));

    printf("The shortest path from 0 to each vertex is:\n");
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

}


/*---------------------------------------------------------------------
 * Function:  Read_n
 * Purpose:   Read in the number of rows in the matrix
 * Ret val:   n:  the number of rows in the matrix
 */
int Read_n() {
    int n;
    n = atoi(file_num);
    /*
    char c;
    int word = 0;
    int count = 0;

    FILE *myFile = fopen("/home/elsa.lopezperez/hpc4ds/graph1.txt", "r");
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
    printf("Elements in the matrix = %d\n", count);
    printf("Value of n = %d\n", n);
    */
    return n;
}  /* Read_n */
