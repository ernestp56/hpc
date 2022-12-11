#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef MAX_BUF
#define MAX_BUF 200
#endif

struct my_record {
    char Processors;
    int Elements;
    int Nodes;
    int loc_n;
    float time;
};

int Read_n();

int main(void) {
    int i;
    int lines = Read_n();
    FILE* my_file = fopen("/home/elsa.lopezperez/hpc4ds/output/output100.csv","r");
    struct my_record records[100];
    size_t count = 0;
    for (; count < sizeof(records)/sizeof(records[0]); ++count){
        int got = fscanf(my_file, "%c,%d,%d,%d,%f", &records[count].Processors, &records[count].Elements, &records[count].Nodes, &records[count].loc_n, &records[count].time);
        if (got != 2) break; // wrong number of tokens - maybe end of file
    }
    fclose(my_file);

    FILE *gnuplot = popen("gnuplot", "w");
    if (!gnuplot) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fprintf(gnuplot, "plot '-' u 1:2 t 'Price' w lp\n");
    for (i = 0; i < lines; ++i) {
        fprintf(gnuplot,"%d %f\n", records[i].Processors, records[i].time);
    }
    fprintf(gnuplot, "e\n");
    fprintf(stdout, "Click Ctrl+d to quit...\n");
    fflush(gnuplot);
    getchar();

    pclose(gnuplot);
    exit(EXIT_SUCCESS);
}

int Read_n(){
    int n;
    //printf("Enter number of vertices in the matrix: \n");
    //scanf("%d", &n);
    char c;
    int word = 0;
    int count = 0;

    char path[MAX_BUF];
    getcwd(path, MAX_BUF);
    char buf[0x100];
    snprintf(buf, sizeof(buf), "%s/hpc4ds/output/output100.csv", path);

    FILE *myFile = fopen(buf, "r");
    //printf("--------500 NODES--------\n");
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
    return n;
}