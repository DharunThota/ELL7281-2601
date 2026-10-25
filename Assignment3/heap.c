//Min heap implementation for the PRIORITYQUEUE ADT, see heap.h for the layout of the structure
#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

//swap the entries at heap positions i and j and keep pos[] in sync
static void heap_swap(HEAP *h, int i, int j){
    int id_i = h->heap[i];
    int id_j = h->heap[j];

    h->heap[i] = id_j;
    h->heap[j] = id_i;
    h->pos[id_j] = i;
    h->pos[id_i] = j;
}

//move the entry at position i up until its parent is not larger
static void heapify_up(HEAP *h, int i){
    while(i > 0){
        int parent = (i - 1) / 2;
        if(h->key[h->heap[parent]] <= h->key[h->heap[i]]){
            break;
        }
        heap_swap(h, parent, i);
        i = parent;
    }
}

//move the entry at position i down until neither child is smaller
static void heapify_down(HEAP *h, int i){
    while(1){
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if(left < h->size && h->key[h->heap[left]] < h->key[h->heap[smallest]]){
            smallest = left;
        }
        if(right < h->size && h->key[h->heap[right]] < h->key[h->heap[smallest]]){
            smallest = right;
        }
        if(smallest == i){
            break;
        }
        heap_swap(h, i, smallest);
        i = smallest;
    }
}

//create an empty queue that can hold ids 0 to capacity-1
HEAP *create_heap(int capacity){
    HEAP *h = (HEAP *)malloc(sizeof(HEAP));
    h->size = 0;
    h->capacity = capacity;
    h->heap = (int *)malloc(capacity * sizeof(int));
    h->pos = (int *)malloc(capacity * sizeof(int));
    h->key = (int *)malloc(capacity * sizeof(int));
    h->n_insert = 0;
    h->n_extract_min = 0;
    h->n_decrease_key = 0;

    for(int i=0;i<capacity;i++){
        h->pos[i] = -1;
    }

    return h;
}

//insert id with the given key, ignored if id is out of range or already in the queue
void heap_insert(HEAP *h, int id, int key){
    if(id < 0 || id >= h->capacity || h->pos[id] != -1){
        return;
    }

    h->key[id] = key;
    h->heap[h->size] = id;
    h->pos[id] = h->size;
    h->size++;
    heapify_up(h, h->size - 1);
    h->n_insert++;
}

//remove the id with the smallest key and return it, returns -1 if the queue is empty
//the key of the removed id stays readable through heap_get_key
int heap_extract_min(HEAP *h){
    if(h->size == 0){
        return -1;
    }

    int min_id = h->heap[0];

    //move the last entry to the root and restore the heap order
    heap_swap(h, 0, h->size - 1);
    h->size--;
    h->pos[min_id] = -1;
    heapify_down(h, 0);
    h->n_extract_min++;

    return min_id;
}

//lower the key of id to new_key, ignored if id is not in the queue or new_key is not smaller
//only a call that actually lowers a key is counted
void heap_decrease_key(HEAP *h, int id, int new_key){
    if(id < 0 || id >= h->capacity || h->pos[id] == -1){
        return;
    }
    if(new_key >= h->key[id]){
        return;
    }

    h->key[id] = new_key;
    heapify_up(h, h->pos[id]);
    h->n_decrease_key++;
}

//1 if the queue has no ids left, 0 otherwise
int heap_is_empty(HEAP *h){
    return h->size == 0;
}

//1 if id is currently in the queue, 0 otherwise
int heap_contains(HEAP *h, int id){
    return id >= 0 && id < h->capacity && h->pos[id] != -1;
}

//current key of id (the last key it had if it was already extracted)
int heap_get_key(HEAP *h, int id){
    return h->key[id];
}

//free the three arrays and the heap itself
void free_heap(HEAP *h){
    free(h->heap);
    free(h->pos);
    free(h->key);
    free(h);
}

//test driver, build with -DTEST_HEAP to run it
#ifdef TEST_HEAP
int main() {
    HEAP *h = create_heap(8);
    printf("Empty: %d\n", heap_is_empty(h));

    heap_insert(h, 0, 50);
    heap_insert(h, 1, 20);
    heap_insert(h, 2, 40);
    heap_insert(h, 3, 10);
    heap_insert(h, 4, 30);
    heap_insert(h, 5, 60);
    printf("Size: %d, contains 3: %d, contains 7: %d\n", h->size, heap_contains(h, 3), heap_contains(h, 7));

    //id 5 jumps to the front, a larger key is ignored
    heap_decrease_key(h, 5, 5);
    heap_decrease_key(h, 0, 70);
    printf("Key of 5: %d, key of 0: %d\n", heap_get_key(h, 5), heap_get_key(h, 0));

    //ids must come out in increasing order of key: 5 3 1 4 2 0
    printf("Extract order:");
    while(!heap_is_empty(h)){
        int id = heap_extract_min(h);
        printf(" %d(key=%d)", id, heap_get_key(h, id));
    }
    printf("\n");

    printf("Extract on empty queue: %d\n", heap_extract_min(h));
    printf("Inserts: %d, extract-mins: %d, decrease-keys: %d\n", h->n_insert, h->n_extract_min, h->n_decrease_key);

    free_heap(h);
    return 0;
}

#endif
