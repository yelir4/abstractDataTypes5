#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<stdbool.h>
#include "pqueue.h"

// coen 12 lab 5, pqueue.c
// 11/17/21

typedef struct pqueue PQ;

// struct pqueue, compare function passed in
struct pqueue {
    int count;          // number of entries in array
    int length;         // length of array
    void **data;
    int (*compare)();   // comparison function
};

/*  *createQueue function
 *  @param compare function
 *  @return pointer to new prio queue
 *  @notation O(1)
 */
PQ *createQueue(int (*compare)()) {
    PQ *pq = malloc(sizeof(PQ));
    assert(pq != NULL);

    pq->count = 0;
    pq->length = 10; // initial length by instructions

    pq->data = malloc(sizeof(void*)*pq->length); // initial array length is 10
    assert(pq->data != NULL);

    pq->compare = compare;
    return pq;
}

/*  destroyQueue function
 *  @param pq passed in pointer to prio queue
 *  deallocates memory associated with pq
 *  @notation O(1)
 */
void destroyQueue(PQ *pq) {
    assert(pq != NULL);

    free(pq->data);
    free(pq);
}

/*  numEntries function
 *  @param pq pointer to prio queue
 *  @return number of entries in pq (pq->count)
 *  @notation O(1)
 */
int numEntries(PQ *pq) {
    assert(pq != NULL);
    return pq->count;
}

/*  addEntry function
 *  @param pq pointer to prio queue
 *  @param entry element to be added to pq
 *  @precondition pq->count is not greater than pq->length (ok if equal)
 *  add entry to pq, ensuring shape is preserved first
 *  followed by sort to restore heap property
 *  double size of array if full
 *  @notation O(logn) amortized cost, O(nlogn) worst case if array length is doubled but we stop adding elements
 */
void addEntry(PQ *pq, void *entry) {
    assert(pq != NULL);
    assert(entry != NULL);
    assert(pq->count <= pq->length);

    // reallocate and double length of array if full
    if(pq->count == pq->length) {
        pq->length *= 2;
        pq->data = realloc(pq->data, sizeof(void*)*pq->length);
    }

    pq->data[pq->count] = entry; // add to left-most open value in array (preserve shape)

    int cur, parent;
    cur = pq->count;
    parent = (cur - 1) / 2; // index of cur's parent (if applicable)

    // re-establish heap property
    while(cur > 0) { // while cur is not the root (and heap property not restored)
        if((*pq->compare)(pq->data[cur], pq->data[parent]) < 0) { // if cur is smaller than its parent
            void *copy = pq->data[parent];
            pq->data[parent] = pq->data[cur];
            pq->data[cur] = copy;
            cur = parent;
            parent = (cur - 1) / 2;
        } else {
            break; // heap property is followed
        }
    }
    pq->count++;
}

/*  *removeEntry function
 *  @param pq pointer to prio queue
 *  @precondition array must not be empty
 *  remove highest priority element (smallest value, we do not choose)
 *  then re-establish heap order
 *  @return previously removed highest prio element
 *  @notation O(logn)
 */
void *removeEntry(PQ *pq) {
    assert(pq != NULL);
    assert(pq->count > 0);

    void *copy = pq->data[0];
    pq->data[0] = pq->data[--pq->count];

    int cur, child, child2;
    cur = 0;
    child = (cur * 2) + 1; // index of cur's first and second child (if applicable)
    child2 = child + 1;

    while(pq->count > child) { // while cur has at least one child
        int smallerChild;
        if(pq->count < child2) // determine which child is smaller
            smallerChild = child;
          else
            smallerChild = ((*pq->compare)(pq->data[child], pq->data[child2]) < 0 ? child : child2);
        
        if((*pq->compare)(pq->data[cur], pq->data[smallerChild]) > 0) { // parent is larger than the smallest child
            void *copy2 = pq->data[cur];
            pq->data[cur] = pq->data[smallerChild];
            pq->data[smallerChild] = copy2;
            cur = smallerChild;
            child = (cur * 2) + 1;
            child2 = child + 1;
        } else {
            break; // heap property followed
        }
    }
    return copy;
}
