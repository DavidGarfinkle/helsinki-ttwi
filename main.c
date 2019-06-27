#include "stdlib.h"
#include "w2.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("USAGE: pattern_path target_path");
        exit(1);
    }

    printf("loading %s\n", argv[1]); fflush(stdout);
    FILE* pattern = fopen(argv[1], "r");
    Score* patternScore = LoadScoreFromFile(pattern);
    fclose(pattern);

    printf("loading %s\n", argv[2]); fflush(stdout);
    FILE* target = fopen(argv[2], "r");
    Score* targetScore = LoadScoreFromFile(target);
    fclose(target);

    printf("searching...\n"); fflush(stdout);
    ResultList* results = search(patternScore, targetScore);

    print_ResultList(results);
}
