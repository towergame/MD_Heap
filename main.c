#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Max size of the buffer, I don't think it's used anymore
#define BUFFER_SIZE 1024

// Struct to keep track of free memory blocks
struct FreeBlock {
    size_t size;
    struct FreeBlock *next;
};

// Linked list of free memory blocks
struct FreeBlock* memory_list = NULL;

// First fit allocation algorithm
int first_fit(size_t size) {
    // iterate through the linked list
    struct FreeBlock* current = memory_list;
    struct FreeBlock* previous = NULL;
    while (current != NULL) {
        if (current->size >= size) {
            // if the block can fit the requested size, slot it in
            if (current->size == size) {
                // If we fill it completely, remove it from the list
                if (previous == NULL) {
                    memory_list = current->next;
                    free(current);
                } else {
                    previous->next = current->next;
                    free(current);
                }
            } else {
                // otherwise reduce the size of the block
                current->size -= size;
            }
            return 0; // successfully allocated
        }
        // move to the next block
        previous = current;
        current = current->next;
    }
    // if we reach the end of the list, we can't allocate the memory
    return -1;
}

// Next fit allocation algorithm
// Keep track of the last block we allocated memory from
struct FreeBlock* current_block = NULL;
struct FreeBlock* previous_block = NULL;
int next_fit(size_t size) {
    // if we haven't allocated any memory yet, start from the beginning
    if(current_block == NULL) {
        current_block = memory_list;
    }
    // iterate through the linked list
    short looped = 0; // keeps track if we have looped through the list
    while (current_block != NULL) {
        if (current_block->size >= size) {
            // if the block can fit the requested size, slot it in
            if (current_block->size == size) {
                // If we fill it completely, remove it from the list
                struct FreeBlock* temp = current_block;
                if (previous_block == NULL) {
                    memory_list = current_block->next;
                } else {
                    previous_block->next = current_block->next;
                    current_block = current_block->next;
                }
                free(temp);
            } else {
                // otherwise reduce the size of the block
                current_block->size -= size;
                current_block = current_block->next;
            }
            return 0; // successfully allocated
        }
        // move to the next block
        previous_block = current_block;
        current_block = current_block->next;
        // loop back to the beginning of the list, if we haven't already
        if (current_block == NULL && !looped) {
            current_block = memory_list;
            looped = 1;
        }
    }
    // if the loop ends, we can't allocate the memory
    return -1;
}

// Best fit allocation algorithm
int best_fit(size_t size) {
    // iterate through the linked list
    struct FreeBlock* current = memory_list;
    struct FreeBlock* previous = NULL;
    // keep track of the best fit and the element before it
    struct FreeBlock* before_best = NULL;
    struct FreeBlock* best = NULL;

    while(current != NULL) {
        if (current->size >= size) {
            // if the block can fit the requested size, check if it's better than the one we have now
            if (best == NULL || current->size < best->size) {
                // if it is, update the best fit
                best = current;
                before_best = previous;
            }
        }

        // move to the next block
        previous = current;
        current = current->next;
    }

    // if we didn't find a block that fits, return -1, since there is no space to allocate
    if (best == NULL) {
        return -1;
    }

    // if the block is exactly the size we need, remove it from the list
    if (best->size == size) {
        if (before_best == NULL) {
            memory_list = best->next;
        } else {
            before_best->next = best->next;
        }
        free(best);
    } else {
        // otherwise reduce the size of the block
        best->size -= size;
    }
    return 0; // successfully allocated
}

// worst fit allocation algorithm
int worst_fit(size_t size) {
    // iterate through the linked list
    struct FreeBlock* current = memory_list;
    struct FreeBlock* previous = NULL;
    // keep track of the worst fit and the element before it
    struct FreeBlock* before_worst = NULL;
    struct FreeBlock* worst = NULL;

    while(current != NULL) {
        // if the block can fit the requested size, check if it's worse than the one we have now
        if (current->size >= size) {
            if (worst == NULL || current->size > worst->size) {
                // if it is, update the worst fit
                worst = current;
                before_worst = previous;
            }
        }

        // move to the next block
        previous = current;
        current = current->next;
    }

    // if we didn't find a block that fits, return -1, since there is no space to allocate
    if (worst == NULL) {
        return -1;
    }

    // if the block is exactly the size we need, remove it from the list
    if (worst->size == size) {
        if (before_worst == NULL) {
            memory_list = worst->next;
        } else {
            before_worst->next = worst->next;
        }
        free(worst);
    } else {
        // otherwise reduce the size of the block
        worst->size -= size;
    }
    return 0; // successfully allocated
}

// random fit allocation algorithm
int rng_fit(size_t size) {
    // count amount of possible candidates
    struct FreeBlock* current = memory_list;
    int count = 0;
    while (current != NULL) {
        if (current->size >= size) {
            count++; // candidate found, increase total by 1
        }
        // move to the next block
        current = current->next;
    }

    // if there are no candidates, return -1, since there is no space to allocate
    if (count == 0) {
        return -1;
    }

    // iterate to a random candidate
    current = memory_list; // reset to start of list
    int index = rand() % count; // pick a candidate at random
    // iterate through the linked list
    while (current != NULL) {
        // if the block can fit the requested size, check if it's the one we want
        if (current->size >= size) {
            if (index == 0) {
                // if it is, allocate the memory
                if (current->size == size) {
                    // If we fill it completely, remove it from the list
                    if (current == memory_list) {
                        memory_list = current->next;
                        free(current);
                    } else {
                        // get the previous block
                        struct FreeBlock* temp = current;
                        struct FreeBlock* previous = memory_list;
                        while (previous->next != current) {
                            previous = previous->next;
                        }
                        // remove the current block from the list
                        previous->next = current->next;
                        free(temp);
                    }
                } else {
                    // otherwise reduce the size of the block
                    current->size -= size;
                }
                return 0; // successfully allocated
            }
            // decrease the index, since we are one candidate nearer to the one we want
            index--;
        }
        // move to the next block
        current = current->next;
    }

    // if we reach the end of the list, we can't allocate the memory, though this should never happen
    return -1;
}

// function to clear the memory list
void clear_memory() {
    // iterate through the linked list
    struct FreeBlock* current = memory_list;
    while (current != NULL) {
        // free the current block and move to the next one
        struct FreeBlock* temp = current;
        current = current->next;
        free(temp);
    }
    // set the memory list to NULL, to indicate it is empty
    memory_list = NULL;
}

// function to initialise the memory list from a file
void initialise_memory(FILE* chunks) {
    // clear the memory list first
    clear_memory();
    // read file line by line
    char* line = NULL;
    size_t len = 0;
    struct FreeBlock* current = NULL;
    while (getline(&line, &len, chunks) != -1) {
        const size_t size = atoi(line); // convert the line to an integer
        if (memory_list == NULL) {
            // if the list is empty, create the first block
            memory_list = malloc(sizeof(struct FreeBlock));
            memory_list->size = size;
            memory_list->next = NULL;
            current = memory_list;
        } else {
            // otherwise, add a new block to the end of the list
            current->next = malloc(sizeof(struct FreeBlock));
            current = current->next;
            current->size = size;
            current->next = NULL;
        }
    }
    // free the line buffer
    if (line != NULL) {
        free(line);
    }
}

// enum to keep track of the allocation algorithms
enum Algorithm {
    FIRST_FIT,
    NEXT_FIT,
    BEST_FIT,
    WORST_FIT,
    RNG_FIT
};

// function to allocate memory using a specific algorithm
int allocate_memory(size_t size, enum Algorithm algorithm) {
    switch (algorithm) {
        case FIRST_FIT:
            return first_fit(size);
        case NEXT_FIT:
            return next_fit(size);
        case BEST_FIT:
            return best_fit(size);
        case WORST_FIT:
            return worst_fit(size);
        case RNG_FIT:
            return rng_fit(size);
        default:
            return -1;
    }
}

// struct to keep track of the memory allocation requests
struct Request {
    size_t size;
    struct Request* next;
};
// linked list of memory allocation requests
struct Request* requests = NULL;

// function to add an allocation request to the list
void add_request(size_t size) {
    // create a new request
    struct Request* new_request = malloc(sizeof(struct Request));
    new_request->size = size;
    new_request->next = NULL;

    // add it to the end of the list
    struct Request* current = requests;
    if (current == NULL) {
        // if the list is empty, set the new request as the first one
        requests = new_request;
    } else {
        // otherwise, add the new request to the end of the list
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_request;
    }
}

// function to clear the list of memory allocation requests
void clear_requests() {
    // iterate through the linked list
    struct Request* current = requests;
    while (current != NULL) {
        // free the current request and move to the next one
        struct Request* temp = current;
        current = current->next;
        free(temp);
    }
}

// function to calculate the fragmentation of the memory list
double calculate_fragmentation() {
    // iterate through the linked list and count the total size and the size of the largest block
    struct FreeBlock* current = memory_list;
    double total = 0;
    double largest = 0;
    while (current != NULL) {
        total += current->size; // add the size of the current block to the total
        if (current->size > largest) {
            // if the current block is the largest, update the largest size
            largest = current->size;
        }
        // move to the next block
        current = current->next;
    }
    // return the fragmentation as a percentage
    if (total == 0) {
        // if the total size is 0, return 1, since there is no free space, which means there can be no fragmentation
        return 1;
    }
    // otherwise use the formula lifted from https://en.wikipedia.org/wiki/Fragmentation_(computing)
    return 1 - (largest / total);
}

// function to print the memory list (used for debugging)
void print_memory() {
    // iterate through the linked list and print the size of each block
    struct FreeBlock* current = memory_list;
    while (current != NULL) {
        // print the size of the current block and move to the next one
        printf("%zu\n", current->size);
        current = current->next;
    }
}

// function to print the memory allocation algorithm name
void print_algorithm(enum Algorithm algorithm) {
    // make the text bold
    printf("\033[1m");
    // print the name of the algorithm
    switch (algorithm) {
        case FIRST_FIT:
            printf("First Fit\n");
            break;
        case NEXT_FIT:
            printf("Next Fit\n");
            break;
        case BEST_FIT:
            printf("Best Fit\n");
            break;
        case WORST_FIT:
            printf("Worst Fit\n");
            break;
        case RNG_FIT:
            printf("Random Fit\n");
            break;
        default:
            // if the algorithm is unknown, print "Unknown Algorithm", since maybe we added a new algorithm and forgot to update this function
            printf("Unknown Algorithm\n");
            break;
    }
    // reset the text to normal
    printf("\033[0m");
}

// function to test a specific algorithm
void test_algorithm(FILE* chunks, enum Algorithm algorithm) {
    // print the name of the algorithm
    print_algorithm(algorithm);
    // reset the file pointer to the beginning of the file (in case it was used before)
    fseek(chunks, 0, SEEK_SET);
    // initialise the memory list from the file
    initialise_memory(chunks);

    // iterate through the linked list of requests and allocate memory for each one
    struct Request* current = requests;
    double time = 0; // keep track of the time taken
    int failed = 0; // keep track of the amount of memory that failed to allocate
    double total = 0; // keep track of the total size of memory requested
    while (current != NULL) {
        const size_t size = current->size; // get the size of the current request
        total += size; // add the size to the total
        struct timespec start={0,0}, end={0,0}; // get ready to record start and end times
        int res; // keep track of the result of the allocation
        clock_gettime(CLOCK_MONOTONIC, &start); // get the time before running the function
        res = allocate_memory(size, algorithm);
        clock_gettime(CLOCK_MONOTONIC, &end); // get the time after running the function
        if (res == -1) {
            // if the allocation failed, add the size to the failed total
            failed += size;
        }
        // calculate the time taken and add it to the total time of all requests
        time += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 10e9;
        // move to the next request
        current = current->next;
    }
    // print the results of the benchmark
    printf("Time taken: %fs\n", time);
    printf("Fragmentation: %.2f%%\n", calculate_fragmentation() * 100);
    double ratio = failed / total; // calculate the ratio of failed memory allocations to total memory allocations
    if (total == 0) {
        // if the total is 0, set the ratio to 0, since that means no memory was requested
        ratio = 0;
    }
    printf("Bytes failed: %d (%.2f%%)\n", failed, ratio);
    printf("\n"); // print a newline to separate the results of different algorithms
}

int main(int argc, char *argv[]) {
    // parse the chunks and sizes files from the command line arguments
    FILE* chunks = NULL;
    FILE* sizes = NULL;

    // iterate through the command line arguments, looking for the chunks and sizes files
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) {
            // if the current argument is -c, the next argument is the chunks file
            chunks = fopen(argv[i + 1], "r");
        } else if (strcmp(argv[i], "-s") == 0) {
            // if the current argument is -s, the next argument is the sizes file
            sizes = fopen(argv[i + 1], "r");
        }
    }

    // if the chunks or sizes files are not provided, print the usage and return 1
    if (chunks == NULL || sizes == NULL) {
        printf("Usage: %s -c <chunks file> -s <sizes file>\n", argv[0]);
        return 1;
    }

    // read the sizes file and add the requests to the list
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, sizes) != -1) {
        const size_t size = atoi(line); // convert the line to an integer
        add_request(size); // add the request to the list
    }
    if (line != NULL) {
        // free the line buffer
        free(line);
    }

    // test each allocation algorithm
    test_algorithm(chunks, FIRST_FIT);
    test_algorithm(chunks, NEXT_FIT);
    test_algorithm(chunks, BEST_FIT);
    test_algorithm(chunks, WORST_FIT);
    srand(time(NULL)); // Seed random with the current time, just to make sure it's *actually* random
    test_algorithm(chunks, RNG_FIT);

    // clear the memory list and the list of requests
    clear_memory();
    clear_requests();

    // close the files
    fclose(sizes);
    fclose(chunks);

    return 0;
}
