#ifndef HEAP_H
#define HEAP_H

//Min priority queue (binary heap) over integer ids 0 to capacity-1, each id carries an integer key
//(for Prim an id is a vertex and the key its cheapest connecting edge, for Kruskal an id is an edge index)
typedef struct _heap{
    int size;       //number of ids currently in the queue
    int capacity;   //ids must lie in 0 to capacity-1
    int *heap;      //heap[i] = id stored at heap position i
    int *pos;       //pos[id] = heap position of id, -1 if id is not in the queue
    int *key;       //key[id] = current key of id
    
    int n_insert;           //number of INSERT operations performed
    int n_extract_min;      //number of EXTRACT-MIN operations performed
    int n_decrease_key;     //number of DECREASE-KEY operations performed
} HEAP;

HEAP *create_heap(int capacity);
void heap_insert(HEAP *h, int id, int key);
int heap_extract_min(HEAP *h);
void heap_decrease_key(HEAP *h, int id, int new_key);
int heap_is_empty(HEAP *h);
int heap_contains(HEAP *h, int id);
int heap_get_key(HEAP *h, int id);
void free_heap(HEAP *h);

#endif
