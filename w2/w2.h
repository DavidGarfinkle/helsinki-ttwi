char* search(char* patternStream, char* targetStream);
void search_return_chains(struct Score* pattern, struct Score* target, struct Result* res);

struct Score* init_score(char* data);

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