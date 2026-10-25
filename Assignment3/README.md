# Assignment 3 - Minimum Spanning Tree (Prim and Kruskal)

Finds the minimum spanning tree of a weighted, undirected graph using Prim's and Kruskal's algorithms,
both written to follow the pseudocode in the course slides (MST.pdf, pages 30 and 57).

## Files

| File | Component |
|---|---|
| `graph.c`, `graph.h` | GRAPH ADT (adjacency matrix) |
| `heap.c`, `heap.h` | PRIORITYQUEUE ADT (binary min heap with INSERT, EXTRACT-MIN, DECREASE-KEY) |
| `union_find.c`, `union_find.h` | Union-Find ADT (disjoint-set forest, union by rank and path compression) |
| `prim_kruskal.c` | Prim's algorithm, Kruskal's algorithm and the top-level program |
| `graph.txt` | sample input (the graph of Fig. 7.4 in AHU) |

## Build

```
gcc prim_kruskal.c graph.c heap.c union_find.c -o mst
```

`graph.c`, `heap.c` and `union_find.c` also contain small test drivers, compiled only on request:

```
gcc -DTEST_GRAPH graph.c -o test_graph
gcc -DTEST_HEAP heap.c -o test_heap
gcc -DTEST_UNION_FIND union_find.c -o test_union_find
```

## Run

```
./mst graph.txt
```

The argument is a text file holding the n x n cost matrix, one row per line, entries separated by spaces:

```
0 6 1 5 0 0
6 0 5 0 3 0
1 5 0 5 6 4
5 0 5 0 0 2
0 3 6 0 0 6
0 0 4 2 6 0
```

- The diagonal is 0 and the matrix is symmetric.
- A missing edge is written as 0 or -1, so both the assignment's format and the 0 format work.
- n is the number of entries in the first line.
- Edge weights must be positive.

## Output

For each algorithm, a header line with its name, the total cost of the tree and its runtime,
followed by the n - 1 edges of the tree as `(u,v)` with vertices numbered from 1, then its operation counts:

```
Prim's algorithm MST, total cost 15, runtime 45.82 microseconds
(1,3)
(2,3)
(2,5)
(3,6)
(4,6)
DECREASE-KEY operations: 8
EXTRACT-MIN operations: 6

Kruskal's algorithm MST, total cost 15, runtime 102.70 microseconds
(1,3)
...
FIND operations: 20
UNION operations: 5
EXTRACT-MIN operations (edge sorting): 10
```

How the operations are counted:

- **DECREASE-KEY**: calls that actually lower a key (Prim).
- **EXTRACT-MIN**: Prim extracts every vertex once. Kruskal extracts every edge once, because it sorts the edges
  by inserting them into the heap and extracting them in order.
- **FIND**: the two `find` calls in Kruskal's test `find(u) != find(v)`, so two per edge examined.
  The finds performed inside a UNION are not counted separately.
- **UNION**: the unions Kruskal performs, one per edge added to the tree.
- **Runtime**: wall-clock time of the algorithm call only, not of reading the file or printing.

## How the modules fit together

The top-level function `run_mst(filename)` in `prim_kruskal.c` reads the matrix into a GRAPH, passes it to
`prim` and `kruskal`, each of which returns a new GRAPH holding the tree, and prints the results.

- **GRAPH**: cells are vertices (0 to n-1) and weighted edges; the aggregate is an n x n matrix; the operations are
  listed at the top of `graph.h`. Prim, Kruskal and the top-level program use the graph only through these functions.
- **Priority queue** (`heap.c`): stores integer ids with integer keys. Prim uses vertices as ids and the cheapest
  connecting edge as the key. Kruskal uses edge indices as ids and the edge weights as keys.
- **Union-Find** (`union_find.c`): used by Kruskal, one set per vertex.

## Notes

- Vertices are numbered from 0 inside the code and from 1 in the printed output.
- A graph with an edge of weight 0 cannot be represented, since 0 marks a missing edge.
- For a disconnected graph the result is a spanning forest, with fewer than n - 1 edges.
