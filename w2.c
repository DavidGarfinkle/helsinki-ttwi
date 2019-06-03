#include "w2.h"

#define W2_DEBUG 0
#define print_debug_1(fmt, ...) \
        do { if (W2_DEBUG >= 1) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); fflush(stderr); } while (0)
#define print_debug_2(fmt, ...) \
        do { if (W2_DEBUG >= 2) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); fflush(stderr); } while (0)

#define print_debug_3(fmt, ...) \
        do { if (W2_DEBUG >= 3) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); fflush(stderr); } while (0)

#define fatal_error(fmt, ...) \
        do { fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); exit(1); } while (0)

int max(int a, int b){
    return (a > b) ? a : b;
}

int min(int a, int b){
    return (a < b) ? a : b;
}

IntraVector NewIntraVector(float x, int y, int startIndex, int endIndex) {
    IntraVector* vec = (IntraVector*) malloc(sizeof(IntraVector));
		if (vec == NULL) {
			fatal_error("%s", "failed to malloc intra vector");
		}
    vec->x = x;
    vec->y = y;
    vec->startIndex = startIndex;
    vec->endIndex = endIndex;
    return *vec;
}

Score* InitScoreFromVectors(int numNotes, int numVecs, IntraVector* vecs) {
		Score* score = (Score*) malloc(sizeof(Score));
		if (score == NULL) {
			fatal_error("%s", "failed to malloc score");
		}
		score->num_notes = numNotes;
		score->num_vectors = numVecs;
		score->vectors = (IntraVector*) calloc(numVecs, sizeof(IntraVector));
		if (score->vectors == NULL) {
			fatal_error("%s", "failed to calloc score->vectors");
		}
		
		for (int i = 0; i < numVecs; i++) {
			score->vectors[i] = vecs[i];
		}

		return score;
}

KEntryNode* NewKEntryNode(IntraVector patternVec, IntraVector targetVec) {
		KEntryNode* node = (KEntryNode*) malloc(sizeof(KEntryNode));
		if (node == NULL) {
			fatal_error("%s", "failed to KEntryNode");
		}
		node->patternVec = patternVec;
		node->targetVec = targetVec;
		node->w = 1;
		node->y = NULL;
		node->next = NULL;
		return node;
}

void print_IntraVector(IntraVector vec) {
	fprintf(stderr, "x: %f, y: %d, startIndex: %d, endIndex: %d",
			vec.x, vec.y, vec.startIndex, vec.endIndex);
}

void print_KEntryNode(KEntryNode* node) {
	fprintf(stderr, "\t----KEntryNode----\n");

	fprintf(stderr, "\tpvec ");
	print_IntraVector(node->patternVec);
	fprintf(stderr, "\n");

	fprintf(stderr, "\ttvec ");
	print_IntraVector(node->targetVec);
	fprintf(stderr, "\n");

	if (node->y != NULL) {
		printf("---- y: \n");
		print_KEntryNode(node->y);
	}
}

void print_KTableLinkedList(KTableLinkedList* list, int num) {
	print_debug_1("KTableLinkedList #%d of length %d\n", num, list->length);
	if (list->length == 0) {
		return;
	}
	KEntryNode* ptr = list->head;
	do {
		print_KEntryNode(ptr);
		ptr = ptr->next;
	} while(ptr != NULL);
}

void print_KTable(KTable table, int num, int length) {
	print_debug_1("KTable #%d of length %d\n", num, length);

	fprintf(stderr, "%-5s | %-5s | %-5s | %-5s | %-5s \t|\t", "idx", "x", "y", "start", "end");
	fprintf(stderr, "\t%-5s | %-5s | %-5s | %-5s | %-5s\n", "idx", "x", "y", "start", "end");
	for (int i=0; i < length; i++) {
		fprintf(stderr, "%-5d | x: %.1f | y: %-2d | si: %-2d | ei: %-1d \t|\t", 
				i, table[i]->patternVec.x, table[i]->patternVec.y, table[i]->patternVec.startIndex, table[i]->patternVec.endIndex);
		fprintf(stderr, "\t%-5d | x: %.1f | y: %-2d | si: %-2d | ei: %-1d\n", 
				i, table[i]->targetVec.x, table[i]->targetVec.y, table[i]->targetVec.startIndex, table[i]->targetVec.endIndex);
	}
}

void print_ResultListNode(ResultListNode* node) {
		printf("[");
		for (int i=0; i < node->length - 1; i++) {
				printf("%d, ", node->chain[i]);
		}
		printf("%d]", node->chain[node->length - 1]);
}

void print_ResultList(ResultList* results) {
		ResultListNode *ptr = results->head;
		printf("total: %d\n", results->length);
		for (int i=0; i < results->length; i++) {
				printf("%d: size %d | ", i, ptr->length);
				print_ResultListNode(ptr);
				printf("\n");
				ptr = ptr->next;
		}
}

bool validIntraVectorMatch(IntraVector patternVec, IntraVector targetVec) {
		bool height_condition = (patternVec.y == targetVec.y);
		bool scale_condition = (patternVec.x == 0 && targetVec.x != 0) || (patternVec.x != 0 && targetVec.x == 0);
		return height_condition && !scale_condition;
}

void pushKTableLinkedList(KTableLinkedList* list, KEntryNode* newNode) {
		if (list->length == 0) {
			list->head = newNode;
			list->tail = newNode;
			list->length = 1;
		} else {
			// Link the list
			list->tail->next = newNode;
			// Then advance the tail
			list->tail = newNode;
			list->length++;
		}
}

int compareKTable(const void* x, const void* y){
		// the caller provides pointers to two list elements
		// list elements are pointers to nodes, so we cast accordingly and dereference
    KEntryNode* left = *((KEntryNode**) x);
    KEntryNode* right = *((KEntryNode**) y);
    if (left->targetVec.startIndex == right->targetVec.startIndex) {
        return left->targetVec.endIndex - right->targetVec.endIndex;
    }
    else {
        return left->targetVec.startIndex - right->targetVec.startIndex;
    }
}
int comparePriorityQueue(const void* x, const void* y){
    KEntryNode* left = ((KEntryNode*) x);
    KEntryNode* right = ((KEntryNode*) y);
    if (left->targetVec.endIndex == right->targetVec.endIndex) {
        return left->targetVec.startIndex < right->targetVec.startIndex;
    }
    else {
        return left->targetVec.endIndex < right->targetVec.endIndex;
    }
}

void InitKTables(KTable* KTables, KTableLinkedList* KLists, Score* pattern, Score* target){
		// The ith KTable holds all vector matches corresponding to pattern vectors starting at index i
		// So there are a total of (pattern->num_notes - 1) tables, since the final table at index M
		// will have pattern vectors starting at index M and ending at index M + 1
		int numTables = pattern->num_notes - 1;

		// Collect the matching vectors into linked lists
    for (int i=0; i < pattern->num_vectors; i++) {
				IntraVector curPatternVec = pattern->vectors[i];
				print_debug_2("matching against pattern vec x: %f, y: %d, startIndex: %d, endIndex: %d\n",
						curPatternVec.x, curPatternVec.y, curPatternVec.startIndex, curPatternVec.endIndex);
        for(int j=0; j < target->num_vectors; j++) {
						IntraVector curTargetVec = target->vectors[j];

						print_debug_3("---- considering x: %f, y: %d, startIndex: %d, endIndex: %d\n",
								curTargetVec.x, curTargetVec.y, curTargetVec.startIndex, curTargetVec.endIndex);

						if (!validIntraVectorMatch(curTargetVec, curPatternVec)) {
								continue;
						}
						print_debug_2("---- matched with x: %f, y: %d, startIndex: %d, endIndex: %d\n",
								curTargetVec.x, curTargetVec.y, curTargetVec.startIndex, curTargetVec.endIndex);

						KTableLinkedList* curList = &KLists[curPatternVec.startIndex];
						KEntryNode* node = NewKEntryNode(curPatternVec, curTargetVec);
						pushKTableLinkedList(curList, node);
        }
		}

		#if W2_DEBUG >= 2
		for (int i=0; i < numTables; i++) {
			print_KTableLinkedList(&KLists[i], i);
		}
		#endif

		// Collapse the linked lists into sortable tables
		for (int i=0; i < numTables; i++) {
				KEntryNode** entries = (KEntryNode**) calloc(KLists[i].length, sizeof(KEntryNode*));
				if (entries == NULL) {
					fatal_error("%s", "failed to calloc KEntryNode** entries");
				}
				// Traverse linked list
				KEntryNode* ptr = KLists[i].head;
				for (int j=0; j < KLists[i].length; j++) {
						entries[j] = ptr;
						ptr = ptr->next;
				}
        qsort(entries, KLists[i].length, sizeof(KEntryNode*), compareKTable);
				KTables[i] = entries;
    }

		#if W2_DEBUG >= 1
		for (int i=0; i < numTables; i++) {
			print_KTable(KTables[i], i, KLists[i].length);
		}
		#endif
}

void enqueueKTable(PQueue** queues, KTable table, int tableLength) {
    for (int i=0; i < tableLength; i++) {
				int index = table[i]->patternVec.endIndex;
				pqueue_enqueue(queues[index], table[i]);
    }
}

PQueue** InitPriorityQueues(int numQueues, int sizeOfQueues) {
    PQueue** queues = (PQueue**) calloc(numQueues, sizeof(PQueue*));
		if (queues == NULL) {
			fatal_error("%s", "failed to malloc list of priority queues");
		}

    for (int i=0; i < numQueues; i++){
        queues[i] = pqueue_new(comparePriorityQueue, sizeOfQueues);
				if (queues[i] == NULL) {
						fatal_error("failed to malloc priority queue %d\n", i);
				}
    }

		return queues;
}

void algorithm(KTable* KTables, KTableLinkedList* KLists, Score* pattern, Score* target){
		// See above in InitKTables
		int numKTables = pattern->num_notes - 1;

		// There are effectively as many priority queues as KTables, but the first one is empty
		// The ith priority queues corresponds to vector pairs whose pattern vector ENDS at index i
		int numPriorityQueues = pattern->num_notes;
		PQueue** queues = InitPriorityQueues(numPriorityQueues, pattern->num_vectors * target->num_vectors);

    // Push the KTables to priority queues
		print_debug_3("%s\n", "queuing ktables");
		for (int i=0; i < numKTables; i++) {
			if (KTables[i][0] == NULL) {
					print_debug_3("skipping ktable %d\n", i);
					continue;
			} else {
					print_debug_3("queuing ktable %d of length %d\n", i, KLists[i].length);
					enqueueKTable(queues, KTables[i], KLists[i].length);
			}
		}

    // For all K tables except the first (already copied to queue) (there are m - 1 Ktables)
    for (int i=1; i < numKTables; i++){
				print_debug_1("k table %d\n", i);

				print_debug_3("priority queue size %zd\n", queues[i]->size);
				if (queues[i]->size == 0) {
					continue;
				}
				KEntryNode* q = (KEntryNode*) pqueue_dequeue(queues[i]);

        // For all rows in the current K Table
        for (int j=0; j < KLists[i].length; j++){

            // Advance the possible antecedent until it matches our first postcedent
            while ((queues[i]->size > 0) && (q->targetVec.endIndex < KTables[i][j]->targetVec.startIndex)) {
                q = (KEntryNode*) pqueue_dequeue(queues[i]);
            }

            if (q->targetVec.endIndex == KTables[i][j]->targetVec.startIndex){
								// For multiple possible antecedents (multiple chains), take the longest one
                while ((queues[i]->size > 0) && (((KEntryNode*)queues[i]->data[0])->targetVec.endIndex == q->targetVec.endIndex)){
                    KEntryNode* r = (KEntryNode*) pqueue_dequeue(queues[i]);
                    if (r->w >= q->w){
                        q = r;
                    }
                }
								// Bind antecedent to postcedent
                KTables[i][j]->w = q->w + 1;
                KTables[i][j]->y = q;
								int queueIndex = KTables[i][j]->patternVec.endIndex;
                pqueue_enqueue(queues[queueIndex], KTables[i][j]);
            }
        }
    }
    for (int i=0; i < numPriorityQueues; i++){
				pqueue_delete(queues[i]);
		}
}

void pushResultList(ResultList* list, ResultListNode* newNode) {
		if (list->length == 0) {
			list->head = newNode;
			list->tail = newNode;
			list->length = 1;
		} else {
			// Link the list
			list->tail->next = newNode;
			// Then advance the tail
			list->tail = newNode;
			list->length++;
		}
}

void extract_chain(KEntryNode* row, int* chain) {
    if (row->y == NULL) {
        chain[0] = row->targetVec.startIndex;
        chain[1] = row->targetVec.endIndex;
    }
    else {
        chain[row->w] = row->targetVec.endIndex;
        // Recurse on the backlink
        extract_chain(row->y, chain);
    }
}

void extract_chains(KTable* KTables, KTableLinkedList* KLists, Score* pattern, Score* target, ResultList* resultList){

    for (int i=0; i < pattern->num_notes - 1; i++) {
				for (int j=0; j < KLists[i].length; j++) {

						if (KTables[i][j]->w < 3) {
								continue;
						}
						int occLength = KTables[i][j]->w + 1;

						ResultListNode* result = (ResultListNode*) malloc(sizeof(ResultListNode));
						result->length = occLength;

						result->chain = (int*)calloc(occLength, sizeof(int));
						if (result->chain == NULL) {
							fatal_error("%s", "failed to malloc chain of integers");
						}

						extract_chain(KTables[i][j], result->chain);

						pushResultList(resultList, result);
				}
    }
}

ResultList* search(Score* pattern, Score* target) {
	print_debug_1("%s\n", "initializing ktables");

	KTableLinkedList* KLists = (KTableLinkedList*) calloc(pattern->num_notes - 1, sizeof(KTableLinkedList));
	if (KLists == NULL) {
		fatal_error("%s", "failed to calloc KLists");
	}

	KTable* KTables = (KTable*)calloc(pattern->num_notes, sizeof(KTable));
	if (KTables == NULL) {
		fatal_error("%s", "failed to calloc KTables");
	}

	InitKTables(KTables, KLists, pattern, target);

	print_debug_1("%s\n", "running algorithm");
	algorithm(KTables, KLists, pattern, target);

	print_debug_1("%s\n", "extracting chains");
	ResultList* resultList = (ResultList*) calloc(1, sizeof(ResultList));
	if (resultList == NULL) {
		fatal_error("%s", "failed to calloc result list");
	}
	extract_chains(KTables, KLists, pattern, target, resultList);

	return resultList;
}
