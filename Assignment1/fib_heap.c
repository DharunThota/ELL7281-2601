#include<stdlib.h>

typedef struct _node{
    int key;                    //value stored in the node
    struct _node *p;            //pointer to parent
    struct _node *child;        //pointer to any child
    struct _node *left, *right; //left and right siblings
    int degree;                 //number of children
} NODE;

typedef struct heap{
    int min;
} FIB_HEAP;

int main(){

    return 0;
}