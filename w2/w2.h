struct Score* initScoreFromVectors(int numNotes, int numVecs, struct IntraVector* vecs);
struct IntraVector NewIntraVector(float x, int y, int startIndex, int endIndex);
int search(struct Score* pattern, struct Score* target, struct Result* results);
void printScore(struct Score* score);

//char* search(char* patternStream, char* targetStream);
void search_return_chains(struct Score* pattern, struct Score* target, struct Result* res);

struct Score* init_score(char* data);
struct Score* init_score_with_length(char* data, int length);
struct Score* init_score_from_int_array(int num_notes, int num_vectors, int* vector_data);
struct Score* init_score_from_vectors(int num_notes, int num_vectors, struct IntraVector* vectors);

struct Score {
    struct IntraVector* vectors;
    int num_notes;
    int num_vectors;
};
 
struct IntraVector {
    float x;
    int y;
    int startIndex;
    int endIndex;
    int startPitch;
    int endPitch;
    int diatonicDiff;
    int chromaticDiff;
};

struct Result{
    int** chains;
    int num_occs;
    struct KEntry* table;
};

struct KEntry {
    struct IntraVector patternVec;
    struct IntraVector targetVec;
    float scale;
    int w; // length of occurrence
    int e;
    struct KEntry* y; // backlink for building chains
};
