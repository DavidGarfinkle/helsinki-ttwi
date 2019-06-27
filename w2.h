#ifndef LIB_W2
#define LIB_W2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "c-generic-pqueue/pqueue.h"

#endif

typedef struct {
    float x;
    int y;
    int startIndex;
    int endIndex;
} IntraVector;

typedef struct {
    IntraVector* vectors;
    int num_notes;
    int num_vectors;
} Score;
 
struct KEntryNode {
    IntraVector patternVec;
    IntraVector targetVec;
    int w; // length of occurrence
    struct KEntryNode* y; // backlink for building chains
		struct KEntryNode* next;
};
typedef struct KEntryNode KEntryNode;

typedef struct {
	KEntryNode* head;
	KEntryNode* tail;
	int length;
} KTableLinkedList;

// K Table is a list of pointers with a sorted order
// The pointers correspond to elements in a KTableLinkedList 
typedef KEntryNode** KTable;

typedef struct {
    int** chains;
    int length;
    KTable* table;
} Result;

struct ResultListNode {
        int* patternNotes;
		int* targetNotes;
		int length;
		struct ResultListNode* next;
};
typedef struct ResultListNode ResultListNode;
typedef struct {
		int length;
		ResultListNode* head;
		ResultListNode* tail;
		ResultListNode* next;
} ResultList;

void printIntArray(int* array, int length);
void print_ResultList(ResultList* results);
Score* LoadScoreFromFile(FILE* data);

extern IntraVector NewIntraVector(float x, int y, int startIndex, int endIndex);
extern Score* InitScoreFromVectors(int numNotes, int numVecs, IntraVector* vecs);
extern ResultList* search(Score* pattern, Score* target);
extern void InitKTables(KTable* KTables,  KTableLinkedList* KLists, Score* pattern, Score* target);
extern void algorithm(KTable* KTables, KTableLinkedList* KLists, Score* pattern, Score* target);
extern void extract_chains(KTable* KTables, KTableLinkedList* KLists, Score* pattern, Score* target, ResultList* resultList);
