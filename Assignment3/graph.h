#ifndef GRAPH_H
#define GRAPH_H

#define NO_EDGE 0  //matrix value for "no edge between these vertices", so edge weights must be positive

//GRAPH ADT: a weighted, undirected graph
//  cells:       a vertex is an integer 0 to n-1, an edge joins two different vertices and carries a positive integer weight
//  aggregation: n x n adjacency matrix, adj_matrix[u][v] is the weight of the edge (u,v) or NO_EDGE,
//               the matrix is symmetric and its diagonal is NO_EDGE
//  operations:  create_graph, create_graph_from_file, add_edge_to_graph, get_number_of_vertices, get_edge_weight,
//               get_neighbours, get_edges, get_total_weight, print_edges, free_graph
//users of the ADT (Prim, Kruskal, the top-level program) must only go through these functions
typedef struct _graph{
    int n;  //number of vertices
    int **adj_matrix;
} GRAPH;

typedef struct _edge{
    int u, v;   //endpoints, 0 indexed
    int weight;
} EDGE;

GRAPH *create_graph(int n);
GRAPH *create_graph_from_file(char *filename);
void add_edge_to_graph(GRAPH *g, int src, int dest, int weight);
int get_number_of_vertices(GRAPH *g);
int get_edge_weight(GRAPH *g, int u, int v);
int get_neighbours(GRAPH *g, int v, int *neighbours, int *weights);
EDGE *get_edges(GRAPH *g, int *count);
int get_total_weight(GRAPH *g);
void print_edges(GRAPH *g);
void free_graph(GRAPH *g);

#endif
