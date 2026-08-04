#include <cstddef>
#include <cstdlib>
#include<stdlib.h>

typedef struct _node{
    int key;                    //value stored in the node
    struct _node *p;            //pointer to parent
    struct _node *child;        //pointer to any child
    struct _node *left, *right; //left and right siblings
    int degree;                 //number of children
    bool mark;
} NODE;

typedef struct heap{
    int n; //number of nodes in heap
    NODE *min; //pointer to minimum node
    NODE *root_list;
} FIB_HEAP;

/*NODE OPERATIONS*/
NODE *create_and_initialize_node(int key){
    NODE *node = (NODE *)malloc(sizeof(NODE));
    node->key = key;
    node->degree = 0;
    node->p = NULL;
    node->child = NULL;
    node->mark = false;
    node->left = node;
    node->right = node;

    return node;
}

/*HEAP OPERATIONS*/
FIB_HEAP *make_fib_heap(){
    FIB_HEAP *temp = (FIB_HEAP *)malloc(sizeof(FIB_HEAP));

    temp->n = 0;
    temp->min = NULL;
    temp->root_list = NULL;

    return temp;
}

void insert_into_root_list(FIB_HEAP *H, NODE *x){
    if(H->root_list == NULL){
        H->root_list = x;
    }
    else{
        x->right = H->root_list;
        x->left = (H->root_list)->left;
        H->root_list->left->right = x;
        H->root_list->left = x;
    }
}

// insert a node with key into the fib heap
void fib_heap_insert(FIB_HEAP *H, int key){
    //creating and initializing a node with given key 
    NODE *x = create_and_initialize_node(key);

    if(H->min == NULL){
        H->min = x;
    }
    else{
        //insert x into H's root list
        insert_into_root_list(H, x);

        //check if inserted node can be the new min
        if(x->key < (H->min)->key){
            H->min = x;
        }
    }

    H->n += 1;
    return;
}

// return minimum node in fib heap
NODE *fib_heap_find_min(FIB_HEAP *H){
    return H->min;
}


int main(){
    FIB_HEAP *h = make_fib_heap();
    return 0;
}