char* search(char* patternStream, char* targetStream);
void search_return_chains(char* patternStream, char* targetStream, struct Result* res);

struct Result{
    int** chains;
    int num_occs;
};