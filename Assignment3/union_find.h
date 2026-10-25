//Disjoint-set Forest interface for UNION-FIND Data Structure
#ifndef UNION_FIND_H
#define UNION_FIND_H

typedef struct _set{
    int rank;
    struct _set *p;
} SET;

SET *make_set(void);
void union_set(SET *, SET *);
void link_set(SET *, SET *);
SET *find_set(SET *);
void free_set(SET *);

#endif
