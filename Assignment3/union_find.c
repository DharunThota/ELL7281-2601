//Disjoint-set Forest implementation for UNION-FIND Data Structure
#include <stdlib.h>
#include "union_find.h"

//initialize new empty set, returns NULL if allocation fails
SET *make_set(void){
    SET *x = (SET *)malloc(sizeof(SET));
    if(x == NULL){
        return NULL;
    }
    x->p = x;
    x->rank = 0;

    return x;
}

//perform union between given sets x, y
void union_set(SET *x, SET *y){
    SET *fx = find_set(x);
    SET *fy = find_set(y);

    //already in the same set: linking a root to itself would only inflate its rank
    if(fx != fy){
        link_set(fx, fy);
    }
}

//link the two given sets x, y (both must be roots)
void link_set(SET *x, SET *y){
    if(x->rank > y->rank){
        y->p = x;
    }
    else{
        x->p = y;
        if(x->rank == y->rank){
            y->rank += 1;
        }
    }
}

//find the set to which given x belongs
SET *find_set(SET *x){
    if(x != x->p){
        x->p = find_set(x->p);
    }
    return x->p;
}

//free the memory of a single element created by make_set
void free_set(SET *x){
    free(x);
}

//test driver, build with -DTEST_UNION_FIND to run it
#ifdef TEST_UNION_FIND
#include <stdio.h>

int main() {
    SET *s[6];
    for(int i=0;i<6;i++){
        s[i] = make_set();
    }

    //every element starts in its own set
    printf("Same set 0,1 before union: %d\n", find_set(s[0]) == find_set(s[1]));

    union_set(s[0], s[1]);
    union_set(s[2], s[3]);
    printf("Same set 0,1 after union: %d\n", find_set(s[0]) == find_set(s[1]));
    printf("Same set 2,3 after union: %d\n", find_set(s[2]) == find_set(s[3]));
    printf("Same set 1,2 (different sets): %d\n", find_set(s[1]) == find_set(s[2]));

    //merging {0,1} with {2,3} joins all four
    union_set(s[1], s[3]);
    printf("Same set 0,2 after merging: %d\n", find_set(s[0]) == find_set(s[2]));
    printf("Same set 0,4 (4 is alone): %d\n", find_set(s[0]) == find_set(s[4]));

    //a union of elements already in the same set must not change the rank
    SET *root = find_set(s[0]);
    int rank_before = root->rank;
    union_set(s[0], s[3]);
    printf("Rank unchanged by repeated union: %d (rank %d)\n", root->rank == rank_before, root->rank);

    //path compression: build the chain c -> b -> a by hand, one find on c points it straight at the root a
    SET *a = make_set(), *b = make_set(), *c = make_set();
    c->p = b;
    b->p = a;
    find_set(c);
    printf("Path compressed (c's parent is the root): %d\n", c->p == a);
    free_set(a);
    free_set(b);
    free_set(c);

    for(int i=0;i<6;i++){
        free_set(s[i]);
    }
    return 0;
}
#endif
