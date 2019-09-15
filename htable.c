#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htable.h"
#include "mylib.h"

/*Initilises variables */
struct htablerec {
    int capacity;
    int num_keys;
    int* frequencies;
    char** keys;
    int* stats;
    hashing_t method;
};


/**
 * Creates a new empty hash table.
 *
 * Returns the new table as result.
 *
 * @param capacity - The capacity of the table.
 * @param h_type - The type of hashing to use; linear or double.
 */
htable htable_new(int capacity, hashing_t h_type) {
    int i;
    htable result = emalloc(sizeof *result);
    result->capacity = capacity;
    result->num_keys = 0;
    result->method = h_type;
    result->frequencies = emalloc(result->capacity * sizeof(result->
                                                            frequencies[0]));
    result->keys = emalloc(result->capacity * sizeof(result->keys[0]));
    result->stats = emalloc(result->capacity * sizeof(result->stats[0]));
    
    for(i = 0; i < result->capacity; i++) {
        result->frequencies[i] = 0;
        result->keys[i] = NULL;
        result->stats[i] = 0;
    }

    return result;
}


/**
 * Used when searching for or inserting values into a hash table
 *
 * @param word  
 */
static unsigned int htable_word_to_int(char *word) {
    unsigned int result = 0;
    
    while (*word != '\0') {
        result = (*word++ + 31 * result);   
    }

    return result;
}

/**
 *  Used when searching for or inserting values into a hash table
 *
 * @param h - the hash table.
 * @param i_key - how far to step
 */
static unsigned int htable_step(htable h, unsigned int i_key) {
    unsigned int result = 1 + (i_key % (h->capacity - 1));
    return result;
}

/**
 * Deallocates the memory from the hash table.
 * So the program can close.
 *
 * @param h - the hash table.
 */
void htable_free(htable h) {
    int i;
    for(i = 0; i < h->capacity; i++) {
        if(h->keys[i] != NULL) { 
            free(h->keys[i]);
        }
    } 

    free(h->frequencies);
    free(h->keys);
    free(h->stats);
    free(h);
}


/**
 * Inserts the words into the hashtable.
 * As defined hashing type (linear or double).
 *
 * @param h - the hash table.
 * @param str - the word to 
 */
int htable_insert(htable h, char *str) {
    int collisions = 0;
    unsigned int key = htable_word_to_int(str);
    unsigned int step = (h->method == LINEAR_P) ? 1 : htable_step(h, key);
    unsigned int index = key % h->capacity;

    while(h->keys[index] != NULL && strcmp(str, h->keys[index]) != 0) {
        index = (index + step) % h->capacity;
        if(index == (key % h->capacity)) {
            return 0;
        }
        collisions++;
    }

    if(h->keys[index] == NULL) {
        h->keys[index] = emalloc((strlen(str) + 1) * sizeof(h->keys[0][0]));
        strcpy(h->keys[index], str);
        h->frequencies[index] = 1; 
        h->stats[h->num_keys] = collisions;
        h->num_keys++;
        return 1;    
    } else {
        h->frequencies[index]++;
        return h->frequencies[index];
    }

    if(collisions == h->capacity) {
        return 0;
    }
}


/**
 * Prints all the frequencies and the keys using a for loop.
 *
 * @param h - the hash table.
 * @param stream - a stream to print the data to.
 */
void htable_print(htable h) {
    int i;
    for(i = 0; i < h->capacity; i++) {
        if(h->keys[i] != NULL) {
            printf("%d  %s\n", h->frequencies[i], h->keys[i]);
        }
    }
}


/**
 * Prints the entire contents of the hashtable.
 *
 * @param h - the hash table.
 */
void htable_print_entire_table(htable h) {
    int count = 0;
    fprintf(stderr, "  Pos  Freq  Stats  Word\n");
    fprintf(stderr, "----------------------------------------\n");
    for(count = 0; count < h-> capacity; count ++){ 
        if(h->keys [count] != NULL){
            fprintf(stderr, "%5d %5d %5d   %s\n",
                    count,
                    h->frequencies[count],
                    h->stats[count],
                    h->keys[count]);
        } else {
            fprintf(stderr,"%5d %5d %5d   \n",
                    count,
                    h->frequencies[count],
                    h->stats[count]);
        }
    }
}

                
/**
 * Search for a word in a given hash table.
 *
 * Returns the word at position index.
 *
 * @param h - the hash table.
 * @param str - the char (int)  to search for.
 */
int htable_search(htable h, char *str) {
    int collisions = 0;
    unsigned int key = htable_word_to_int(str);
    unsigned int index = key % h->capacity;
    unsigned int step = (h->method == LINEAR_P) ? 1 : htable_step(h, key);
    while(h->keys[index] != NULL && strcmp(str, h->keys[index]) != 0) {
        index = (index + step) % h->capacity;
        collisions++;

        
        if(index == (key % h->capacity)) {
            break;
        }
    }   

    if(collisions == h->capacity) {
        return 0;
    }

    return h->frequencies[index];
}



/*************FUNCTIONS GIVEN TO US*************/

/**
 * Prints out a line of data from the hash table to reflect the state
 * the table was in when it was a certain percentage full.
 * Note: If the hashtable is less full than percent_full then no data
 * will be printed.
 *
 * @param h - the hash table.
 * @param stream - a stream to print the data to.
 * @param percent_full - the point at which to show the data from.
 */
static void print_stats_line(htable h, FILE *stream, int percent_full) {
    int current_entries = h->capacity * percent_full / 100;
    double average_collisions = 0.0;
    int at_home = 0;
    int max_collisions = 0;
    int i = 0;

    if (current_entries > 0 && current_entries <= h->num_keys) {
        for (i = 0; i < current_entries; i++) {
            if (h->stats[i] == 0) {
                at_home++;
            } 
            if (h->stats[i] > max_collisions) {
                max_collisions = h->stats[i];
            }
            average_collisions += h->stats[i];
        }
    
        fprintf(stream, "%4d %10d %11.1f %10.2f %11d\n", percent_full, 
                current_entries, at_home * 100.0 / current_entries,
                average_collisions / current_entries, max_collisions);
    }
}


/**
 * Prints out a table showing what the following attributes were like
 * at regular intervals (as determined by num_stats) while the
 * hashtable was being built.
 *
 * @li Percent At Home - how many keys were placed without a collision
 * occurring.
 * @li Average Collisions - how many collisions have occurred on
 *  average while placing all of the keys so far.
 * @li Maximum Collisions - the most collisions that have occurred
 * while placing a key.
 *
 * @param h the hashtable to print statistics summary from.
 * @param stream the stream to send output to.
 * @param num_stats the maximum number of statistical snapshots to print.
 */
void htable_print_stats(htable h, FILE *stream, int num_stats) {
    int i;

    fprintf(stream, "\n%s\n\n", 
            h->method == LINEAR_P ? "Linear Probing" : "Double Hashing"); 
    fprintf(stream, "Percent   Current    Percent    Average      Maximum\n");
    fprintf(stream, " Full     Entries    At Home   Collisions   Collisions\n");
    fprintf(stream, "------------------------------------------------------\n");
    for (i = 1; i <= num_stats; i++) {
        print_stats_line(h, stream, 100 * i / num_stats);
    }
    fprintf(stream, "------------------------------------------------------\n\n"
            );
}
