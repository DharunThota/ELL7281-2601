#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define D_N 100


/*STRUCT DEFINITIONS*/
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

/*FUNCTION DECLARATIONS*/
void fib_heap_insert(FIB_HEAP *H, NODE *x);


/*UTILITIES*/
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

void remove_from_root_list(FIB_HEAP *H, NODE *x){
    if(x == x->right){
        H->min = NULL;
        H->root_list = NULL;
    }

    if(x == H->root_list){
        H->root_list = x->right;
    }
    x->left->right = x->right;
    x->right->left = x->left;
}

void insert_into_child_list(NODE *parent, NODE *new_child){
    if(parent->child){
        new_child->right = parent->child;
        new_child->left = parent->child->left;
        parent->child->left->right = new_child;
        parent->child->left = new_child;
    }
    new_child->p = parent;
    parent->child = new_child;
    parent->degree++;
    return;
}

void remove_from_child_list(NODE *parent, NODE *c){
    if(parent->child == parent->child->right){
        //only one child
        parent->child = NULL;
    }
    else if(parent->child == c){
        parent->child = c->right;
    }
    c->left->right = c->right;
    c->right->left = c->left;
    parent->degree--;
}

void fib_heap_link(FIB_HEAP *H, NODE *y, NODE *x){
    //remove y from root list
    remove_from_root_list(H, y);

    //make y child of x, increment degree of x
    insert_into_child_list(x,y);
    
    y->mark = false;

    return;
}

void compare_degrees(FIB_HEAP *H, NODE **A, NODE *x){
    int d = x->degree;
    while(A[d]){
        // if(d > D_N - 1) return;

        NODE *y = A[d];
        if(x == y) break;

        if(x->key > y->key){
            //swap x and y
            NODE *temp = x;
            x = y;
            y = temp;
        }
        fib_heap_link(H, y, x);
        A[d] = NULL;
        d++;
    }
    A[d] = x;
}

void consolidate(FIB_HEAP *H){
    //D(n) = O(logn)
    NODE **A = (NODE **)malloc(D_N * sizeof(NODE));
    memset(A, '\0', D_N);
    
    NODE *x = H->root_list;
    NODE *last = x->left;
    //for each node in the root list
    while(x != last){
        NODE *next = x->right;
        compare_degrees(H, A, x);
        x = next;
    }
    //for the last node; missed in the while loop
    compare_degrees(H, A, last);

    H->min = NULL;
    for(int i=0;i<D_N;i++){
        if(A[i]){
            fib_heap_insert(H, A[i]);
        }
    }

    free(A);
    return;
}

void cut(FIB_HEAP *H, NODE *x, NODE *y){
    // remove x from child list of y
    remove_from_child_list(y, x);

    //insert x into root list
    insert_into_root_list(H, x);

    x->p = NULL;
    x->mark = false;
    return;
}

void cascading_cut(FIB_HEAP *H, NODE *y){
    NODE *z = y->p;
    if(z){
        if(!y->mark){
            y->mark = true;
        }
        else{
            cut(H, y, z);
            cascading_cut(H, z);
        }
    }
    return;
}

/*HEAP OPERATIONS*/
FIB_HEAP *make_fib_heap(){
    FIB_HEAP *temp = (FIB_HEAP *)malloc(sizeof(FIB_HEAP));

    temp->n = 0;
    temp->min = NULL;
    temp->root_list = NULL;

    return temp;
}


// insert a node with key into the fib heap
void fib_heap_insert(FIB_HEAP *H, NODE *x){
    if(H->min == NULL){
        H->min = x;
        insert_into_root_list(H, x);
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

NODE *fib_heap_extract_min(FIB_HEAP *H){
    NODE *z = H->min;
    if(z != NULL){
        //TODO 1: add each child of z to root list
        if(z->child != NULL){
            NODE *head = z->child;
            NODE *curr = head;

            do {
                NODE *next = curr->right;
                curr->left->right = curr->right;
                curr->right->left = curr->left;

                curr->left = curr;
                curr->right = curr;

                curr->p = NULL;
                insert_into_root_list(H, curr);
                curr = next;

            }while (curr != head);

            z->child = NULL;
            z->degree = 0;
        }

        //TODO 2: remove z from root list
        remove_from_root_list(H, z);
        H->min = z->right;
        consolidate(H);

        H->n -= 1;
    }
    return z;
}

void fib_heap_decrease_key(FIB_HEAP *H, NODE *x, int new_key){
    if(new_key > x->key){
        printf("new key greater than current key");
        return;
    }

    x->key = new_key;
    NODE *y = x->p;
    if(y && (x->key < y->key)){
        cut(H, x, y);
        cascading_cut(H, y);
    }
    if(x->key < H->min->key){
        H->min = x;
    }
    
    return;
}

int main(){
    FIB_HEAP *h = make_fib_heap();
    //creating and initializing a node with given key
    int key = 5; 
    NODE *x = create_and_initialize_node(key);
    fib_heap_insert(h, x);

    printf("Success");
    return 0;
}