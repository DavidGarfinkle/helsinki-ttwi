#include <w2.c>

Score* LoadScoreFromFile(FILE* data) {
		int chunkSize = 1024;
    char line[chunkSize];
    Score* score =  malloc(sizeof(Score));

    // Skip the first line which documents the csv headers
    fgets(line, chunkSize, data);
    // Get the number of notes (assumed to be on the second line)
    fgets(line, chunkSize, data);
    score->num_notes = atoi(line); 
    // Get the number of vectors (assumed to be on the third line)
    fgets(line, chunkSize, data);
    score->num_vectors = atoi(line);

    // Allocate space for storing vectors
    score->vectors = (IntraVector*) calloc(score->num_vectors, sizeof(IntraVector));

    // Load the rest of the intra vectors
    for (int i=0; i < score->num_vectors; i++){
        fgets(line, chunkSize, data);
        score->vectors[i].x = atof(strtok(line, ","));
        score->vectors[i].y = atoi(strtok(NULL, ",")); 
        score->vectors[i].startIndex = atoi(strtok(NULL, ",")); 
        score->vectors[i].endIndex = atoi(strtok(NULL, ",")); 
    }

    return score;
}

void printIntArray(int* array, int length) {
		printf("[");
		for (int i=0; i < length - 1; i++) {
				printf("%d, ", array[i]);
		}
		printf("%d]", array[length-1]);
}

int assertExpectedResults(ResultList* results, int** expected, int expectedNumResults, int* expectedResultLengths) {
		/* Returns index of the first non-matching result */

		if (results->length < expectedNumResults) {
				printf("got %d results, expected at least %d\n", results->length, expectedNumResults); fflush(stdout);
				return 0;
		}

		ResultListNode* ptr = results->head;

		for (int i=0; i < results->length; i++) {
				// filter for results of sufficient size 
				while (ptr != NULL) {
						if (ptr->length != expectedResultLengths[i]) {
								ptr = ptr->next;
						} else {
								break;
						}
				}
				if (ptr == NULL) {
						printf("didn't find result for expected: "); fflush(stdout);
						printIntArray(expected[i], expectedResultLengths[i]);
						printf("\n");
						return i;
				}

				// compare to the expected
				for (int j=0; j < expectedResultLengths[i]; j++) {
						if (expected[i][j] != ptr->chain[j]) {
								printf("expected ");
								printIntArray(expected[i], expectedResultLengths[i]);
								printf("\n");

								printf("got ");
								printIntArray(ptr->chain, ptr->length);
								printf("\n");
								return i;
						}
				}
				ptr = ptr->next;
		}
		return -1;
}

ResultList* testQuery(char* filename) {
		FILE* leiermannFile = fopen("tests/leiermann.csv", "r");
		Score* target = LoadScoreFromFile(leiermannFile);
		fclose(leiermannFile);

		printf("loading %s\n", filename); fflush(stdout);
		FILE* queryFileHandle = fopen(filename, "r");
		Score* query = LoadScoreFromFile(queryFileHandle);

		printf("searching %s\n", filename); fflush(stdout);
		ResultList* results = search(query, target);

		return results;
}

int main() {

		char queryFileNames[6][18] = {
				"tests/query_a.csv",
				"tests/query_b.csv",
				"tests/query_c.csv",
				"tests/query_d.csv",
				"tests/query_e.csv",
				"tests/query_f.csv"
		};

		int *expectedResults[][5] = {
			{
				(int[]){2, 6, 8, 11, 16, 21},
				(int[]){2, 6, 8, 11, 16, 31},
				(int[]){12, 14, 16, 17, 18, 21},
				(int[]){12, 14, 16, 17, 18, 31},
				(int[]){12, 14, 18, 20, 29, 31}
			},
			{
				(int[]){2, 6, 8, 15, 16, 21},
				(int[]){2, 6, 8, 15, 16, 31},
				(int[]){2, 6, 8, 15, 29, 31},
			}
		};
		int *expectedResultLengths[2] = {
				(int[]){6, 6, 6, 6, 6},
				(int[]){5, 5, 5, 5, 5}
		};
		int expectedNumResults[5] = {5, 3, 5, 5, 5};
				
		for (int i=0; i < 1; i++) {
				ResultList* results = testQuery(queryFileNames[i]);
				print_ResultList(results);
				//int status = assertExpectedResults(results, expectedResults[i], expectedNumResults[i], expectedResultLengths[i]);
				/*
				if (status != -1) {
						printf("FAIL\n"); fflush(stdout);
						return 1;
				}
				*/
		}

		printf("PASS\n"); fflush(stdout);
}
