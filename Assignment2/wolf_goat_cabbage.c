/*
 * Wolf, Goat, Cabbage river crossing problem.
 * Modelled as a state space search. Each state is a node in a tree
 * (linked using parent and child pointers). We grow the tree from the
 * start state by trying every legal move, never revisiting a state that
 * already occurred on the same path (this keeps the tree cycle free).
 * Every root to leaf path that ends in the goal state is one way of
 * solving the puzzle.
 */

#include <stdio.h>
#include <stdlib.h>

#define LEFT 0
#define RIGHT 1
#define MAX_CHILDREN 4   /* person alone, person+wolf, person+goat, person+cabbage */

/* a state just says which bank each of the 4 things is on */
typedef struct State {
    int person;
    int wolf;
    int goat;
    int cabbage;
} State;

/* linked representation of a state and the transitions out of it */
typedef struct Node {
    State state;
    struct Node *parent;
    struct Node *child[MAX_CHILDREN];
    int numChild;
    int isGoal;
} Node;

int totalWays = 0;

int sameState(State a, State b)
{
    return a.person == b.person && a.wolf == b.wolf &&
           a.goat == b.goat && a.cabbage == b.cabbage;
}

/* unsafe if wolf+goat alone together, or goat+cabbage alone together */
int isSafe(State s)
{
    if (s.wolf == s.goat && s.person != s.wolf)
        return 0;
    if (s.goat == s.cabbage && s.person != s.goat)
        return 0;
    return 1;
}

int isGoalState(State s)
{
    return s.person == RIGHT && s.wolf == RIGHT &&
           s.goat == RIGHT && s.cabbage == RIGHT;
}

/* walk up the parent chain to see if this state already occurred
   earlier on the same path -- this is how we avoid cycles */
int onPathAlready(Node *node, State s)
{
    while (node != NULL) {
        if (sameState(node->state, s))
            return 1;
        node = node->parent;
    }
    return 0;
}

Node *makeNode(State s, Node *parent)
{
    Node *n = (Node *) malloc(sizeof(Node));
    int i;

    n->state = s;
    n->parent = parent;
    n->numChild = 0;
    n->isGoal = isGoalState(s);
    for (i = 0; i < MAX_CHILDREN; i++)
        n->child[i] = NULL;
    return n;
}

/* try all 4 possible moves out of node->state and recurse on the
   ones that are legal, safe, and don't repeat a state on this path */
void buildTree(Node *node)
{
    int move;

    if (node->isGoal) {
        totalWays++;
        return;
    }

    for (move = 0; move < MAX_CHILDREN; move++) {
        State s = node->state;
        int otherBank = 1 - s.person;

        /* move 0 = go alone, 1 = take wolf, 2 = take goat, 3 = take cabbage */
        if (move == 1 && s.wolf != s.person)
            continue;
        if (move == 2 && s.goat != s.person)
            continue;
        if (move == 3 && s.cabbage != s.person)
            continue;

        s.person = otherBank;
        if (move == 1) s.wolf = otherBank;
        if (move == 2) s.goat = otherBank;
        if (move == 3) s.cabbage = otherBank;

        if (!isSafe(s))
            continue;
        if (onPathAlready(node, s))
            continue;

        node->child[node->numChild] = makeNode(s, node);
        buildTree(node->child[node->numChild]);
        node->numChild++;
    }
}

char *bankName(int bank)
{
    return bank == LEFT ? "left " : "right";
}

void printState(State s)
{
    printf("    person: %s   wolf: %s   goat: %s   cabbage: %s\n",
           bankName(s.person), bankName(s.wolf),
           bankName(s.goat), bankName(s.cabbage));
}

/* print the states from the root down to this node, using the
   parent pointers to walk back up first */
void printPath(Node *node)
{
    if (node->parent != NULL)
        printPath(node->parent);
    printState(node->state);
}

int wayCount = 0;

/* number of moves taken to reach this node = number of ancestors */
int depthOf(Node *node)
{
    int depth = 0;
    while (node->parent != NULL) {
        depth++;
        node = node->parent;
    }
    return depth;
}

/* find every goal node in the tree and print the path to it */
void printSolutions(Node *node)
{
    int i;

    if (node->isGoal) {
        wayCount++;
        printf("\nWay %d (%d crossings):\n", wayCount, depthOf(node));
        printPath(node);
        return;
    }
    for (i = 0; i < node->numChild; i++)
        printSolutions(node->child[i]);
}

int main(void)
{
    State start = {LEFT, LEFT, LEFT, LEFT};
    Node *root = makeNode(start, NULL);

    buildTree(root);
    printSolutions(root);

    printf("\nTotal number of ways to cross the river: %d\n", totalWays);
    return 0;
}
