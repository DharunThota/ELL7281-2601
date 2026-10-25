#define _POSIX_C_SOURCE 199309L  //for clock_gettime
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "graph.h"
#include "heap.h"
#include "union_find.h"

//Prim's and Kruskal's algorithms for the minimum spanning tree, and the top-level program that runs both

//stands in for the infinity of the pseudocode, larger than any edge weight
#define INFINITY_KEY INT_MAX

//Prim's algorithm, as in the course slides (page 30)
//Q = V is kept in a min heap, the key of v is the weight of the least-weight edge connecting v to the tree
GRAPH *prim(GRAPH *g, int *n_decrease_key, int *n_extract_min){
    int n = get_number_of_vertices(g);
    int s = 0;  //root of the tree, any vertex works

    int *parent = (int *)malloc(n * sizeof(int));   //parent[v] = pi[v], -1 if none
    int *neighbours = (int *)malloc(n * sizeof(int));   //adjacent vertices of the vertex being processed
    int *weights = (int *)malloc(n * sizeof(int));      //weights[i] = weight of the edge to neighbours[i]
    HEAP *Q = create_heap(n);

    //Q <- V, key[v] <- infinity for all v, key[s] <- 0
    for(int v=0;v<n;v++){
        parent[v] = -1;
        heap_insert(Q, v, (v == s) ? 0 : INFINITY_KEY);
    }

    while(!heap_is_empty(Q)){
        //u is the vertex outside the tree that is cheapest to connect
        int u = heap_extract_min(Q);

        int n_neigh = get_neighbours(g, u, neighbours, weights);
        for(int i=0;i<n_neigh;i++){
            int v = neighbours[i];
            int w = weights[i];

            //if v in Q and w(u,v) < key[v] then key[v] <- w(u,v) (DECREASE-KEY), pi[v] <- u
            if(heap_contains(Q, v) && w < heap_get_key(Q, v)){
                heap_decrease_key(Q, v, w);
                parent[v] = u;
            }
        }
    }

    //at the end {(v, pi[v])} forms the MST, copy those edges into a new graph
    GRAPH *mst = create_graph(n);
    for(int v=0;v<n;v++){
        if(parent[v] != -1){
            add_edge_to_graph(mst, v, parent[v], get_edge_weight(g, v, parent[v]));
        }
    }

    //report the operation counts kept by the heap
    *n_decrease_key = Q->n_decrease_key;
    *n_extract_min = Q->n_extract_min;

    free_heap(Q);
    free(parent);
    free(neighbours);
    free(weights);

    return mst;
}

//Kruskal's algorithm
//the edges are sorted by inserting them into a min heap (id = edge index, key = weight)
//and extracting them one by one
GRAPH *kruskal(GRAPH *g, int *n_find, int *n_union, int *n_extract_min){
    int n = get_number_of_vertices(g);
    int m;  //number of edges
    EDGE *edges = get_edges(g, &m);

    //FIND and UNION operations are counted here, around the calls to the union-find functions
    *n_find = 0;
    *n_union = 0;

    //sort E by weight in non-decreasing order
    HEAP *sorted = create_heap(m > 0 ? m : 1);
    for(int i=0;i<m;i++){
        heap_insert(sorted, i, edges[i].weight);
    }

    //MST = {}
    GRAPH *mst = create_graph(n);

    //for v in V: makeSet(v), sets[v] is the element that represents vertex v
    SET **sets = (SET **)malloc(n * sizeof(SET *));
    for(int v=0;v<n;v++){
        sets[v] = make_set();
    }

    //for (u,v) in E: if find(u) != find(v) then add (u,v) to MST, union(u,v)
    while(!heap_is_empty(sorted)){
        //the next lightest edge
        EDGE e = edges[heap_extract_min(sorted)];

        //two FINDs, one for each endpoint
        SET *set_u = find_set(sets[e.u]);
        SET *set_v = find_set(sets[e.v]);
        *n_find += 2;

        if(set_u != set_v){
            add_edge_to_graph(mst, e.u, e.v, e.weight);
            union_set(sets[e.u], sets[e.v]);
            *n_union += 1;
        }
    }

    //report the extract-min count kept by the heap, then free everything except the MST
    *n_extract_min = sorted->n_extract_min;

    for(int v=0;v<n;v++){
        free_set(sets[v]);
    }
    free(sets);
    free_heap(sorted);
    free(edges);

    return mst;
}

//time elapsed between two timestamps, in microseconds
static double elapsed_us(struct timespec start, struct timespec end){
    return (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3;
}

//top-level function: read the cost matrix from the given file, build the graph,
//run Prim and Kruskal on it and print both MSTs along with the operation counts and runtimes
void run_mst(char *filename){
    GRAPH *g = create_graph_from_file(filename);
    struct timespec start, end;

    //Prim
    int n_decrease_key, prim_extract_min;
    clock_gettime(CLOCK_MONOTONIC, &start);
    GRAPH *prim_mst = prim(g, &n_decrease_key, &prim_extract_min);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double prim_time = elapsed_us(start, end);

    //Kruskal
    int n_find, n_union, kruskal_extract_min;
    clock_gettime(CLOCK_MONOTONIC, &start);
    GRAPH *kruskal_mst = kruskal(g, &n_find, &n_union, &kruskal_extract_min);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double kruskal_time = elapsed_us(start, end);

    printf("Prim's algorithm MST, total cost %d, runtime %.2f microseconds\n", get_total_weight(prim_mst), prim_time);
    print_edges(prim_mst);
    printf("DECREASE-KEY operations: %d\n", n_decrease_key);
    printf("EXTRACT-MIN operations: %d\n\n", prim_extract_min);

    printf("Kruskal's algorithm MST, total cost %d, runtime %.2f microseconds\n", get_total_weight(kruskal_mst), kruskal_time);
    print_edges(kruskal_mst);
    printf("FIND operations: %d\n", n_find);
    printf("UNION operations: %d\n", n_union);
    printf("EXTRACT-MIN operations (edge sorting): %d\n", kruskal_extract_min);

    free_graph(prim_mst);
    free_graph(kruskal_mst);
    free_graph(g);
}

//expects the name of the cost matrix file as the only argument
int main(int argc, char **argv) {
    if(argc != 2){
        fprintf(stderr, "usage: %s <cost matrix file>\n", argv[0]);
        return 1;
    }

    run_mst(argv[1]);
    return 0;
}
