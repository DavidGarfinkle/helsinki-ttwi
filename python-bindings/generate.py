import os
from cffi import FFI
ffibuilder = FFI()

root_dir = os.path.abspath(os.path.join(__file__, os.path.pardir, os.path.pardir))

# cdef() expects a single string declaring the C types, functions and
# globals needed to use the shared object. It must be in valid C syntax.
ffibuilder.cdef("""
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
""")

# set_source() gives the name of the python extension module to
# produce, and some C source code as a string.  This C code needs
# to make the declarated functions, types and globals available,
# so it is often just the "#include".
ffibuilder.set_source("_w2",
"""
     #include "w2.h"   // the C header of the library
""",
     libraries=['w2'], library_dirs=[os.path.join(root_dir, "build")]   # library name, for the linker
)

if __name__ == "__main__":
    ffibuilder.compile(verbose=True)
