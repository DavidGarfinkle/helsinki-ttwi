#include <w2.c>

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
						if (expected[i][j] != ptr->targetNotes[j]) {
								printf("expected ");
								printIntArray(expected[i], expectedResultLengths[i]);
								printf("\n");

								printf("got ");
								printIntArray(ptr->targetNotes, ptr->length);
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


        int *expected[50] = {
            (int[]){3, 6, 8, 9},
            (int[]){3, 6, 8, 11},
            (int[]){3, 6, 8, 12},
            (int[]){3, 6, 8, 13},
            (int[]){3, 6, 8, 16},
            (int[]){3, 6, 8, 17},
            (int[]){3, 6, 8, 18},
            (int[]){3, 6, 8, 20},
            (int[]){3, 6, 8, 21},
            (int[]){3, 6, 8, 29},
            (int[]){3, 6, 8, 31},
            (int[]){13, 14, 16, 17},
            (int[]){13, 14, 16, 18},
            (int[]){13, 14, 16, 20},
            (int[]){13, 14, 16, 21},
            (int[]){13, 14, 16, 29},
            (int[]){13, 14, 16, 31},
            (int[]){13, 14, 18, 20},
            (int[]){13, 14, 18, 21},
            (int[]){13, 14, 18, 29},
            (int[]){13, 14, 18, 31},
            (int[]){24, 25, 26, 28},
            (int[]){21, 27, 29, 31},
            (int[]){3, 6, 8, 9, 12},
            (int[]){3, 6, 8, 9, 13},
            (int[]){3, 6, 8, 9, 16},
            (int[]){3, 6, 8, 9, 18},
            (int[]){3, 6, 8, 9, 21},
            (int[]){3, 6, 8, 9, 29},
            (int[]){3, 6, 8, 9, 31},
            (int[]){3, 6, 8, 11, 16},
            (int[]){3, 6, 8, 11, 18},
            (int[]){3, 6, 8, 11, 21},
            (int[]){3, 6, 8, 11, 29},
            (int[]){3, 6, 8, 11, 31},
            (int[]){13, 14, 16, 17, 18},
            (int[]){13, 14, 16, 17, 21},
            (int[]){13, 14, 16, 17, 29},
            (int[]){13, 14, 16, 17, 31},
            (int[]){13, 14, 18, 20, 29},
            (int[]){13, 14, 18, 20, 31},
            (int[]){3, 6, 8, 12},
            (int[]){3, 6, 8, 13},
            (int[]){3, 6, 8, 21},
            (int[]){3, 6, 8, 31},
            (int[]){3, 6, 8, 11, 16, 21},
            (int[]){3, 6, 8, 11, 16, 31},
            (int[]){13, 14, 16, 17, 18, 21},
            (int[]){13, 14, 16, 17, 18, 31},
            (int[]){13, 14, 18, 20, 29, 31}
        };
        int expectedLength[50] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 6, 6, 6, 6, 6};


        /*
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
        */
				
        int queries_to_try[3] = {0, 2, 4};
		for (int i=0; i < 3; i++) {
				ResultList* results = testQuery(queryFileNames[queries_to_try[i]]);
				print_ResultList(results);
				int status = assertExpectedResults(results, expected, 50, expectedLength);
				if (status != -1) {
						printf("FAIL on result %d\n", status); fflush(stdout);
						return 1;
				}
		}

		printf("PASS\n"); fflush(stdout);
}
