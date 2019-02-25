import os
from cffi import FFI
ffibuilder = FFI()

cur_dir = os.path.abspath(os.path.join(__file__, os.path.pardir))

# cdef() expects a single string declaring the C types, functions and
# globals needed to use the shared object. It must be in valid C syntax.
ffibuilder.cdef("""
    char* search(char* patternString, char* targetString);
    void search_return_chains(char* patternString, char* targetString, struct Result* res);


     struct Result {
          int** chains;
          int num_occs;
     };

     /**
     * Priority Queue Structure
     */
     typedef struct PQueue_s {
     /* The actual size of heap at a certain time */
     size_t size;
     /* The amount of allocated memory for the heap */
     size_t capacity;
     /* An array of (void*), the actual max-heap */
     void **data;
     /* A pointer to a comparator function, used to prioritize elements */
     int (*cmp)(const void *d1, const void *d2);
     } PQueue;

     /** Allocates memory for a new Priority Queue .
     Needs a pointer to a comparator function, thus establishing priorities .
     */
     PQueue *pqueue_new(int (*cmp)(const void *d1, const void *d2),
                    size_t capacity);

     /** De-allocates memory for a given Priority Queue */
     void pqueue_delete(PQueue *q);

     /** Add an element inside the Priority Queue */
     void pqueue_enqueue(PQueue *q, const void *data);

     /** Removes the element with the greatest priority from within the Queue */
     void *pqueue_dequeue(PQueue *q);
""")

# set_source() gives the name of the python extension module to
# produce, and some C source code as a string.  This C code needs
# to make the declarated functions, types and globals available,
# so it is often just the "#include".
ffibuilder.set_source("_w2",
f"""
     #include "{os.path.join(cur_dir, "w2.h")}"   // the C header of the library
     #include "{os.path.join(cur_dir, "c_pqueue/pqueue.h")}"
""",
     libraries=['w2'], library_dirs=[cur_dir])   # library name, for the linker

if __name__ == "__main__":
    ffibuilder.compile(verbose=True) 