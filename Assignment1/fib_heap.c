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
void print_fib_heap(FIB_HEAP *H);


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
        x->left = x;
        x->right = x;
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
    else{
        new_child->left = new_child;
        new_child->right = new_child;
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
    if(H->root_list == NULL) return;

    NODE **A = (NODE **)malloc(D_N * sizeof(NODE));
    memset(A, '\0', D_N * sizeof(NODE));
    
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
    H->root_list = NULL;
    for(int i=0;i<D_N;i++){
        if(A[i]){
            insert_into_root_list(H, A[i]);
            if(H->min == NULL || A[i]->key < H->min->key){
                H->min = A[i];
            }
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

/*VISUALIZATION*/
 
#define MAX_TRAVERSE 10000 // safety cap: a well-formed heap never gets close to this
 
// counts the number of siblings in a circular doubly linked list, starting at 'start'.
// stops early (and flags it) if the list doesn't cycle back within MAX_TRAVERSE steps,
// which indicates a corrupted circular list rather than looping forever
static int count_siblings(NODE *start){
    if(!start) return 0;
    int count = 0;
    NODE *curr = start;
    do{
        count++;
        curr = curr->right;
    }while(curr != start && count < MAX_TRAVERSE);
 
    if(count >= MAX_TRAVERSE){
        printf("  [!] WARNING: sibling list did not cycle back to start after %d nodes -- likely a corrupted circular list\n", MAX_TRAVERSE);
    }
    return count;
}
 
// recursively prints a node and its children using tree-drawing connectors,
// e.g.:
// ├── 5 (degree=2)
// │   ├── 8 [marked]
// │   └── 12
// └── 7 (MIN)
#define MAX_DEPTH 200      // safety cap: no legitimate fibonacci-heap tree nests this deep
#define MAX_PRINT_SIBLINGS 50 // safety cap: never print more than this many siblings at one level
 
static void print_fib_tree(NODE *node, NODE *min, const char *prefix, bool is_last, int depth){
    printf("%s%s", prefix, is_last ? "\xe2\x94\x94\xe2\x94\x80\xe2\x94\x80 " : "\xe2\x94\x9c\xe2\x94\x80\xe2\x94\x80 ");
 
    printf("%d", node->key);
    if(node == min)   printf("  <-- MIN");
    if(node->mark)    printf("  [marked]");
    printf("  (degree=%d)\n", node->degree);
 
    if(depth >= MAX_DEPTH){
        printf("%s    [!] WARNING: hit max recursion depth (%d) -- likely a parent/child cycle, stopping here\n",
               prefix, MAX_DEPTH);
        return;
    }
 
    if(node->child){
        char new_prefix[512];
        snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix,
                 is_last ? "    " : "\xe2\x94\x82   ");
 
        int total = count_siblings(node->child);
        int print_limit = total < MAX_PRINT_SIBLINGS ? total : MAX_PRINT_SIBLINGS;
        int idx = 0;
        NODE *curr = node->child;
        do{
            idx++;
            print_fib_tree(curr, min, new_prefix, idx == print_limit, depth + 1);
            curr = curr->right;
        }while(curr != node->child && idx < print_limit);
 
        if(total > MAX_PRINT_SIBLINGS){
            printf("%s    [!] ... %d more children not shown (truncated)\n", new_prefix, total - MAX_PRINT_SIBLINGS);
        }
    }
}
 
// recursively walks every node in the heap (root list + all descendants)
// and collects the ones with mark == true
static void collect_marked_nodes_impl(NODE *start, NODE **out, int *count, int max, int depth){
    if(!start || depth >= MAX_DEPTH) return;
    NODE *curr = start;
    int steps = 0;
    do{
        if(curr->mark && *count < max){
            out[(*count)++] = curr;
        }
        if(curr->child){
            collect_marked_nodes_impl(curr->child, out, count, max, depth + 1);
        }
        curr = curr->right;
        steps++;
    }while(curr != start && steps < MAX_TRAVERSE);
}
 
static void collect_marked_nodes(NODE *start, NODE **out, int *count, int max){
    collect_marked_nodes_impl(start, out, count, max, 0);
}
 
// prints a summary line of every marked node currently in the heap
void print_marked_nodes(FIB_HEAP *H){
    NODE *marked[1024];
    int count = 0;
 
    if(H->root_list){
        collect_marked_nodes(H->root_list, marked, &count, 1024);
    }
 
    printf("Marked nodes (%d): ", count);
    if(count == 0){
        printf("none\n");
    }
    else{
        for(int i = 0; i < count; i++){
            printf("%d%s", marked[i]->key, (i < count - 1) ? ", " : "\n");
        }
    }
}
 
// prints the whole fibonacci heap: each root-list tree drawn with its
// children indented underneath, the current min highlighted, and marked
// nodes flagged inline as well as summarized at the end
void print_fib_heap(FIB_HEAP *H){
    printf("========================================\n");
    printf("Fibonacci Heap  (n = %d)\n", H->n);
    printf("========================================\n");
 
    if(H->root_list == NULL){
        printf("(empty heap)\n\n");
        return;
    }
 
    printf("Min node: %d\n\n", H->min ? H->min->key : -1);
    printf("Root list:\n");
 
    int total = count_siblings(H->root_list);
    int print_limit = total < MAX_PRINT_SIBLINGS ? total : MAX_PRINT_SIBLINGS;
    int idx = 0;
    NODE *curr = H->root_list;
    do{
        idx++;
        print_fib_tree(curr, H->min, "", idx == print_limit, 0);
        curr = curr->right;
    }while(curr != H->root_list && idx < print_limit);
 
    if(total > MAX_PRINT_SIBLINGS){
        printf("    [!] ... %d more root-list trees not shown (truncated)\n", total - MAX_PRINT_SIBLINGS);
    }
 
    printf("\n");
    print_marked_nodes(H);
    printf("\n");
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
        //add each child of z to root list
        if(z->child != NULL){
            NODE *curr = z->child;
            int n_child = z->degree;

            for(int i=0;i<n_child;i++){
                NODE *next = curr->right;
                curr->p = NULL;
                insert_into_root_list(H, curr);
                curr = next;
            }

            z->child = NULL;
            z->degree = 0;
        }

        //remove z from root list
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
 
    int keys[] = {5, 3, 17, 24, 7, 18, 52, 38, 30, 26, 46};
    int nkeys = sizeof(keys) / sizeof(keys[0]);
 
    for(int i = 0; i < nkeys; i++){
        NODE *node = create_and_initialize_node(keys[i]);
        fib_heap_insert(h, node);
    }
 
    printf("After inserting %d keys:\n", nkeys);
    print_fib_heap(h);
 
    NODE *extracted = fib_heap_extract_min(h);
    printf("Extracted min: %d\n\n", extracted->key);
 
    printf("After extract_min (note the merged trees from consolidate):\n");
    print_fib_heap(h);
 
    // manually mark a node just to demonstrate how marked nodes are rendered
    if(h->root_list && h->root_list->child){
        h->root_list->child->mark = true;
        printf("Manually marked node %d for demonstration:\n", h->root_list->child->key);
        print_fib_heap(h);
    }

    extracted = fib_heap_extract_min(h);
    printf("Extracted min: %d\n\n", extracted->key);
 
    printf("After extract_min (note the merged trees from consolidate):\n");
    print_fib_heap(h);
    

    return 0;
}