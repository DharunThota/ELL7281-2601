//GRAPH ADT implementation, see graph.h for a description of the ADT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

#define MAX_LENGTH 4096

//read an n x n cost matrix from the file (0 or -1 for no edge), n is the number of entries in the first line
GRAPH *create_graph_from_file(char *filename){
    FILE *fptr;
    char buffer[MAX_LENGTH];
    int n = 0;
    
    fptr = fopen(filename, "r");
    if(fptr == NULL){
        exit(1);
    }

    //count the entries in the first line to get the number of vertices
    if(fgets(buffer, sizeof(buffer), fptr) != NULL){
        char copy_buffer[MAX_LENGTH];
        strcpy(copy_buffer, buffer);

        char *token = strtok(copy_buffer, " \t\r\n");
        while (token != NULL) {
            n++;
            token = strtok(NULL, " \t\r\n");
        }
    }

    //allocate the n x n matrix, one row at a time
    int **adj_matrix = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        adj_matrix[i] = (int *)malloc(n * sizeof(int));
    }

    //go back to the start and read the matrix
    rewind(fptr);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(fptr, "%d", &adj_matrix[i][j]);

            //the input marks a missing edge with 0 or -1, both are stored as NO_EDGE
            if(adj_matrix[i][j] < 0){
                adj_matrix[i][j] = NO_EDGE;
            }
        }
    }
    fclose(fptr);

    GRAPH *g = (GRAPH *)malloc(sizeof(GRAPH));
    g->n = n;
    g->adj_matrix = adj_matrix;

    return g;
}

//create a graph with n vertices and no edges (every matrix entry is NO_EDGE)
GRAPH *create_graph(int n){
    int **adj_matrix = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        adj_matrix[i] = (int *)calloc(n, sizeof(int));
    }

    GRAPH *g = (GRAPH *)malloc(sizeof(GRAPH));
    g->n = n;
    g->adj_matrix = adj_matrix;

    return g;
}

//add the undirected edge (src, dest) with the given weight, vertices are numbered 0 to n-1
//the weight must be positive since 0 marks a missing edge
void add_edge_to_graph(GRAPH *g, int src, int dest, int weight){
    if(src == dest){
        return;
    }

    if(weight <= 0){
        return;
    }

    int n = g->n;
    if(src < 0 || dest < 0 || src >= n || dest >= n){
        return;
    }

    g->adj_matrix[src][dest] = weight;
    g->adj_matrix[dest][src] = weight;
    return;
}

//number of vertices in the graph
int get_number_of_vertices(GRAPH *g){
    return g->n;
}

//weight of the edge (u, v), or NO_EDGE if there is none
int get_edge_weight(GRAPH *g, int u, int v){
    return g->adj_matrix[u][v];
}

//fill neighbours[] and weights[] with the vertices adjacent to v and the weight of each edge,
//both arrays must have room for n entries, returns the number of neighbours
int get_neighbours(GRAPH *g, int v, int *neighbours, int *weights){
    int count = 0;
    for(int u=0;u<g->n;u++){
        if(u != v && g->adj_matrix[v][u] != NO_EDGE){
            neighbours[count] = u;
            weights[count] = g->adj_matrix[v][u];
            count++;
        }
    }

    return count;
}

//return an array of all edges, each undirected edge once with u < v,
//the number of edges is stored in *count, the caller must free the array
EDGE *get_edges(GRAPH *g, int *count){
    int n = g->n;
    int m = 0;
    EDGE *edges = (EDGE *)malloc((n * (n - 1) / 2 + 1) * sizeof(EDGE));

    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            if(g->adj_matrix[i][j] != NO_EDGE){
                edges[m].u = i;
                edges[m].v = j;
                edges[m].weight = g->adj_matrix[i][j];
                m++;
            }
        }
    }

    *count = m;
    return edges;
}

//sum of the weights of all edges, each edge counted once
int get_total_weight(GRAPH *g){
    int total_weight = 0;
    for(int i=0;i<g->n;i++){
        for(int j=i+1;j<g->n;j++){
            total_weight += g->adj_matrix[i][j];
        }
    }

    return total_weight;
}

//print each edge as (u,v) on its own line, vertices numbered from 1
void print_edges(GRAPH *g){
    for(int i=0;i<g->n;i++){
        for(int j=i+1;j<g->n;j++){
            if(g->adj_matrix[i][j] != NO_EDGE){
                printf("(%d,%d)\n", i+1, j+1);
            }
        }
    }
}

//print the adjacency matrix, used by the test driver
void print_graph(GRAPH *g){
    for(int i=0;i<g->n;i++){
        for(int j=0;j<g->n;j++){
            printf("%d ", g->adj_matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

//free the rows of the matrix, then the matrix, then the graph itself
void free_graph(GRAPH *g){
    for (int i = 0; i < g->n; i++) {
        free(g->adj_matrix[i]);
    }
    free(g->adj_matrix);
    free(g);
}

//test driver, build with -DTEST_GRAPH to run it
#ifdef TEST_GRAPH
int main() {
    GRAPH *g = create_graph_from_file("graph.txt");
    print_graph(g);

    add_edge_to_graph(g, 0, 4, 7);
    printf("%d\n", get_edge_weight(g, 0, 4));

    int n = get_number_of_vertices(g);
    printf("Number of vertices: %d\n", n);

    int *neighbours = (int *)malloc(n * sizeof(int));
    int *weights = (int *)malloc(n * sizeof(int));
    int n_neigh = get_neighbours(g, 0, neighbours, weights);
    printf("Number of neighbours of 0: %d\n", n_neigh);
    for(int i=0;i<n_neigh;i++){
        printf("%d: %d\n", neighbours[i], weights[i]);
    }

    printf("Total weight of graph: %d\n", get_total_weight(g));

    int n_edge;
    EDGE *edge_list = get_edges(g, &n_edge);
    for(int i=0;i<n_edge;i++){
        EDGE edge = edge_list[i];
        printf("%d -> %d: %d\n", edge.u, edge.v, edge.weight);
    }

    print_graph(g);
    free_graph(g);
    return 0;
}
#endif
