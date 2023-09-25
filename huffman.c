#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<stdbool.h>
#include<ctype.h>
#include "pqueue.h"
#include "pack.h"

// coen 12 lab 5, huffman.c
// 11/17/21

typedef struct pqueue PQ; // from pqueue.c
typedef struct node NODE; // from pack.h

/* intcmp function
 * @param *n1, *n2 node pointers with counts to be compared
 * @return -1 if n1 is less than or equal to n2, 1 otherwise
 */
static int nodecmp(NODE *n1, NODE *n2) {
    assert(n1 != NULL);
    assert(n2 != NULL);
    return (n1->count < n2->count) ? -1 : (n1->count > n2->count);
}

/* depth function
 * @return depth of node n1 passed in
 */
static int depth(NODE *n1) {
    assert(n1 != NULL);
    NODE *nCur = n1;
    int i = 0;
    while(nCur->parent != NULL) {
        nCur = nCur->parent;
        i++;
    }
    return i;
}

/* main function
 * count occurences of characters in a file passed in,
 * form priority queue and determine how many bits the file will take
 * output this information and then compress the file 
 */
int main(int argc, char *argv[]) {
    // terminate if insufficient files passed in
    if(argc != 3) {
        printf("missing file name");
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    // terminate if file is invalid
	if(fp==NULL) {
		printf("must enter a valid file\n");
		return 1;
	}

    int count[257]; // array of characters
    NODE *nodes[257]; // parallel array of nodes
    int i;
    for(i=0; i<256; i++) {
        count[i] = 0;
        nodes[i] = NULL;
    }

    int c; // c is current character
    while((c=getc(fp)) != EOF) // count number of occurrences for each character in the file
        count[c]++;

    fclose(fp); // close file

    // allocate node with nonzero counts and pass them into the priority queue
    PQ *pq = createQueue(nodecmp);
    for(i=0; i<256; i++)
        if(count[i] != 0) {
            nodes[i] = malloc(sizeof(NODE));
            assert(nodes[i] != NULL);

            nodes[i]->count = count[i];
            nodes[i]->parent = NULL;
            addEntry(pq, nodes[i]);
        }
    
    nodes[256] = malloc(sizeof(NODE)); // EOF character
    assert(nodes[256] != NULL);
    nodes[256]->count = 0;
    nodes[256]->parent = NULL;
    addEntry(pq, nodes[256]);

    // while there is more than one node in the prio queue
    while(numEntries(pq) > 1) {
        NODE *c1 = malloc(sizeof(NODE));
        NODE *c2 = malloc(sizeof(NODE));
        NODE *parent = malloc(sizeof(NODE));
        assert(c1 != NULL);
        assert(c2 != NULL);
        assert(parent != NULL);

        // remove the two nodes with smallest count, combine into one
        c1 = removeEntry(pq);
        c2 = removeEntry(pq);
        c1->parent = parent;
        c2->parent = parent;
        parent->count = (c1->count + c2->count);
        addEntry(pq, parent);
    }

    // print out table of information with depth and count of each character
    for(i=0; i<256; i++) {
        if(count[i] != 0) {
            if(isprint(i)!=0) // if given character is printable, print, otherwise print octal value
                printf("'%c'", i);
                else
                printf("%o", i);
            printf(": %d x %d bits = %d bits\n", nodes[i]->count, depth(nodes[i]), (depth(nodes[i]) * nodes[i]->count));
        }
    }

    destroyQueue(pq);
    pack(argv[1], argv[2], nodes);

    for(i=0; i<257; i++)
        free(nodes[i]);
    return 0;
}
