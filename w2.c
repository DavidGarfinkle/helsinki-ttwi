#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "c_pqueue/pqueue.h"

#define W2_DEBUG 1
#define debug_print(fmt, ...) \
        do { if (W2_DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); fflush(stderr); } while (0)

int max(int a, int b){
    return (a > b) ? a : b;
}

int min(int a, int b){
    return (a < b) ? a : b;
}

struct Score {
    struct IntraVector* vectors;
    int num_notes;
    int num_vectors;
};

struct Result {
    int** chains;
    int num_occs;
    struct KEntry* table;
};

struct IntraVector {
    float x;
    int y;
    int startIndex;
    int endIndex;

		int chromaticDiff;
		int diatonicDiff;
		int startPitch;
		int endPitch;

};

struct KEntry {
    struct IntraVector patternVec;
    struct IntraVector targetVec;
    float scale;
    int w; // length of occurrence
		int f; // final
    int e;
    struct KEntry* y; // backlink for building chains
};

struct IntraVector NewIntraVector(float x, int y, int startIndex, int endIndex) {
    struct IntraVector* vec = (struct IntraVector*) malloc(sizeof(struct IntraVector));
    vec->x = x;
    vec->y = y;
    vec->startIndex = startIndex;
    vec->endIndex = endIndex;
    return *vec;
}

void print_K_table(struct KEntry* KTable){
	int i = 0;
	while (KTable[i].f != 1) {
        printf("KEntry %d; y %d; tStart %d; tEnd %d; pStart %d; pEnd %d; w %d; ", i, KTable[i].targetVec.y, KTable[i].targetVec.startIndex, KTable[i].targetVec.endIndex, KTable[i].patternVec.startIndex, KTable[i].patternVec.endIndex, KTable[i].w);
        if (KTable[i].w > 1 && KTable[i].y != NULL) {
            printf("y-endindex %d\n", KTable[i].y->targetVec.endIndex);
        } else {
            printf("\n");
        }
				i++;
    }
}

void printKTables(struct KEntry** KTables, int length){
	for (int k = 0; k < length; k++) {
		printf("\nKTABLE %d\n", k); fflush(stdout);
		print_K_table(KTables[k]);
	}
}

struct Score* initScoreFromVectors(int numNotes, int numVecs, struct IntraVector* vecs) {
	struct Score* score = (struct Score*) malloc(sizeof(struct Score));
	score->num_notes = numNotes;
	score->num_vectors = numVecs;
	score->vectors = (struct IntraVector*) calloc(numVecs, sizeof(struct IntraVector));
	
	for (int i = 0; i < numVecs; i++) {
		score->vectors[i] = vecs[i];
	}

	return score;
}

struct Score* load_indexed_score(FILE* data){
    char line[1024];
    struct Score* score =  malloc(sizeof(struct Score));

    // Skip the first line which documents the csv headers
    fgets(line, 1024, data);
    // Get the number of notes (assumed to be on the second line)
    fgets(line, 1024, data);
    score->num_notes = atoi(line); 
    // Get the number of vectors (assumed to be on the third line)
    fgets(line, 1024, data);
    score->num_vectors = atoi(line);

    // Allocate space for storing vectors
    // (use malloc to create space that is persistent beyond function scope)
    score->vectors = (struct IntraVector *) malloc(sizeof(struct IntraVector) * score->num_vectors);

    // Load the rest of the intra vectors
    for (int i=0; i<score->num_vectors; i++){
        fgets(line, 1024, data);
        score->vectors[i].x = atof(strtok(line, ","));
        score->vectors[i].y = atoi(strtok(NULL, ",")); 
        score->vectors[i].startIndex = atoi(strtok(NULL, ",")); 
        score->vectors[i].endIndex = atoi(strtok(NULL, ",")); 
        score->vectors[i].startPitch = atoi(strtok(NULL, ",")); 
        score->vectors[i].endPitch = atoi(strtok(NULL, ",")); 
        //strcpy(score->vectors[i].startPitch, strtok(NULL, ",")); 
        //strcpy(score->vectors[i].endPitch, strtok(NULL, ",")); 
        score->vectors[i].diatonicDiff = atoi(strtok(NULL, ",")); 
        score->vectors[i].chromaticDiff = atoi(strtok(NULL, ",")); 
    }

    return score;
}

struct Score* init_score(char* data) {
    FILE* stream = fmemopen(data, strlen(data), "r");
    return load_indexed_score(stream);
}

void extract_chain(struct KEntry row, int* chain, int* maxTargetWindow, int* transposition, int* diatonicOcc) {
    int curTargetWindow = row.targetVec.endIndex - row.targetVec.startIndex; 
    if (curTargetWindow > *maxTargetWindow) *maxTargetWindow = curTargetWindow;

    if ((row.targetVec.chromaticDiff != row.patternVec.chromaticDiff) && (row.targetVec.diatonicDiff == row.patternVec.diatonicDiff)) {
        *diatonicOcc = 1;
    }

    if (row.y == NULL) {
        chain[0] = row.targetVec.startIndex;
        chain[1] = row.targetVec.endIndex;

        *transposition = row.patternVec.startPitch - row.targetVec.startPitch;
    }
    else {
        chain[row.w + 1] = row.targetVec.endIndex;
        // Recurse on the backlink
        extract_chain(*(row.y), chain, maxTargetWindow, transposition, diatonicOcc);
    }
}

int extract_chains(struct KEntry** KTables, struct Score* pattern, struct Score* target, int** chains){
    int num_occs = 0;

    for (int i=0; i < target->num_vectors; i++){
        int transposition = 0;
        int maxTargetWindow = 0;
        int diatonicOcc = 0;

        // Full occurrence will be m - 1 vectors, indexed at 0 ==> check for m - 2
        if (KTables[pattern->num_notes - 2][i].w != pattern->num_notes - 2) {
            continue;
        }

        int* chain = calloc(pattern->num_notes, sizeof(int));

        extract_chain(KTables[pattern->num_notes - 2][i], chain, &maxTargetWindow, &transposition, &diatonicOcc);
        chains[num_occs] = chain;

        num_occs++;
    }

    return num_occs;
}

int compare_K_entries_startIndex(const void* x, const void* y){
    struct KEntry left = *((struct KEntry*) x);
    struct KEntry right = *((struct KEntry*) y);
    if (left.targetVec.startIndex == right.targetVec.startIndex) {
        return left.targetVec.endIndex - right.targetVec.endIndex;
    }
    else {
        return left.targetVec.startIndex - right.targetVec.startIndex;
    }
}
int compare_K_entries_endIndex(const void* x, const void* y){
    struct KEntry left = *((struct KEntry*) x);
    struct KEntry right = *((struct KEntry*) y);
    if (left.targetVec.endIndex == right.targetVec.endIndex) {
        return left.targetVec.startIndex < right.targetVec.startIndex;
    }
    else {
        return left.targetVec.endIndex < right.targetVec.endIndex;
    }
}

struct KEntry** init_K_tables(struct Score* pattern, struct Score* target){

    // Each K table is a list of K entries
    // We have as many K tables as there are notes in the pattern
    struct KEntry** KTables = (struct KEntry **) malloc(pattern->num_vectors * sizeof(struct KEntry*));

    for (int i=0; i < pattern->num_notes - 1; i++) {
				// For now, allocate the max possible size of any K table
        KTables[i] = (struct KEntry *) calloc(target->num_vectors, sizeof(struct KEntry));

        // For W1 only. Filter out for the single pattern vec which goes from i to i + 1
        struct IntraVector curPatternVec;
        for (int m=0; m < pattern->num_vectors; m++){
            curPatternVec = pattern->vectors[m];
            if ((curPatternVec.startIndex == i) && (curPatternVec.endIndex == i + 1)){
                break;
            }
        }

        // Find all db vectors which match the Pi -> Pi+1
        // Could spead up by taking advantage of sorted vectors, or by hashing to y value
        int numMatching = 0;
        for(int j=0; j < target->num_vectors; j++){
            if (target->vectors[j].y == curPatternVec.y){
                KTables[i][numMatching].targetVec = target->vectors[j];
                KTables[i][numMatching].patternVec = curPatternVec;
                KTables[i][numMatching].y = NULL;
                //KTables[i][numMatching].w = 1;
                numMatching++;
            }
        }
        qsort(KTables[i], numMatching, sizeof(struct KEntry), compare_K_entries_startIndex);
				KTables[i][numMatching].f = 1;
    }
		//printKTables(KTables, pattern->num_notes - 1);
    return KTables;
}

void freeKTables(struct KEntry** tables, int num_tables, int table_length) {
	for(int i=0; i < num_tables; i++) {
		for(int j=0; j < table_length; j++) {
			free(&tables[i][j]);
			printf("free %d\n", j); fflush(stdout);
		}
		printf("free %d\n", i); fflush(stdout);
		free(tables[i]);
	}
}

void algorithm(struct KEntry** KTables, struct Score* pattern, struct Score* target){
    // TODO find optimal size of K Tables and queues
    //struct KEntry*** Queues = malloc(pattern->num_notes * sizeof(struct KEntry**));
    PQueue** Queues = (PQueue**) malloc(pattern->num_notes * sizeof(PQueue*));
    for (int i=0; i < pattern->num_notes; i++){
        Queues[i] = pqueue_new(compare_K_entries_endIndex, target->num_vectors);
    }

    // Initialize the first Queue. These are lists of pointers to KEntries
    for (int i=0; KTables[0][i].f != 1; i++){
        // TODO don't add empty KTable rows. find optimal size of KTables and queues..
        if (KTables[0][i].targetVec.startIndex != 0){
            pqueue_enqueue(Queues[1], &KTables[0][i]);
        }
    }


    struct KEntry* q;
    // For all K tables except the first (already copied to queue) (there are m - 1 Ktables)
    for (int i=1; i <= pattern->num_notes - 2; i++){
				debug_print("k table %d\n", i);
        if (Queues[i]->size > 0){
            q = (struct KEntry *) pqueue_dequeue(Queues[i]);
        } else {
					//continue;
				}
        
        // For all rows in the current K Table
        for (int j=0; KTables[i][j].f != 1; j++){
						//:debug printf("	k row %d\n", j);fflush(stdout);
            // Advance the possible antecedent until it matches our first postcedent
            while (q->targetVec.endIndex < KTables[i][j].targetVec.startIndex && Queues[i]->size > 0){
                q = (struct KEntry *) pqueue_dequeue(Queues[i]);
            }

            if (q->targetVec.endIndex == KTables[i][j].targetVec.startIndex){

                // For multiple possible antecedents (multiple chains), take the longest one
                while (((struct KEntry*)Queues[i]->data[0])->targetVec.endIndex == q->targetVec.endIndex && Queues[i]->size > 0){
                    struct KEntry* r = (struct KEntry*) pqueue_dequeue(Queues[i]);
                    if (r->w >= q->w){
                        q = r;
                    }
                }
                KTables[i][j].w = q->w + 1;
                KTables[i][j].y = q;
                pqueue_enqueue(Queues[i+1], &KTables[i][j]);

                if (Queues[i]->size > 0) {
                    q = (struct KEntry*) pqueue_dequeue(Queues[i]);
                }
            }
        }
        KTables[i][target->num_vectors-1].e = 1;
        pqueue_enqueue(Queues[i+1], &KTables[i][target->num_vectors-1]);
    }
    for (int i=0; i < pattern->num_notes; i++){
			pqueue_delete(Queues[i]);
		}
}

int search(struct Score* pattern, struct Score* target, struct Result* results) {
	//:debug printf("tables\n"); fflush(stdout);
	struct KEntry ** KTables = init_K_tables(pattern, target);

	//:debug printf("algorithm\n");fflush(stdout);
	algorithm(KTables, pattern, target);

	//:debug printf("chain\n");fflush(stdout);
	results->chains = calloc(target->num_notes, sizeof(int*));
	results->num_occs = extract_chains(KTables, pattern, target, results->chains);

	//:debug printf("free\n");fflush(stdout);

	return 0;
}


void search_return_chains(struct Score* pattern, struct Score* target, struct Result* res){

    struct KEntry** KTables = init_K_tables(pattern, target);

    algorithm(KTables, pattern, target);

    res->chains = calloc(target->num_notes, sizeof(int*));
    res->num_occs = extract_chains(KTables, pattern, target, res->chains);
}
/*

void print_queue(PQueue* queue){
    for (int i=0; i < queue->size; i++){
        struct KEntry* q = (struct KEntry *) pqueue_dequeue(queue);
        printf("targetvec startindex %d\n", q->targetVec.startIndex);
    }
}

void printScore(struct Score* score){
    printf("%d notes\n", score->num_notes);
    printf("%d vectors\n", score->num_vectors);
    for (int i=0; i < score->num_vectors; i++){
        printf("x: %f, y: %d, startIndex: %d, endIndex: %d\n", score->vectors[i].x, score->vectors[i].y, score->vectors[i].startIndex, score->vectors[i].endIndex);
    }
}

*/
