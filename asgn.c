#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include "htable.h"
#include "mylib.h"
#include <string.h>

/* The default hashtable size */
#define SIZE 113

/*
* The main method of the program. It takes arguments from the terminal and
* depending on the argument, cases will be executed accordingly. 
* 
*/
int main(int argc, char **argv){
    htable h;
    char word[256];
    hashing_t hashtype = LINEAR_P;
    char *text_filename = emalloc(sizeof(char) * (4096 + 1)); 
    
    int table_size = SIZE;

    int c_flag=0, e_flag=0, p_flag=0, s_flag=0, h_flag=0;

    clock_t start, end;
    double search_time=0.0, fill_time=0.0;
    int unknown = 0;

    int snapshots=0, length=0;

   
    const char *optstring = "c:deps:t:h";
    char option;
    
    /*
    * This checks to see what the user inputs, and sets the case flags to 1 or 
    * 0 accordingly. If no input is given, 
    *
    * -c prints all unknown words to stdout
    * -d uses double hashing as the collision resolution strategy
    * -e displays entire contents of hash table
    * -p prints a list of stats and the type of hashing provided
    * -s displays the given number of stats snapshots
    * -t Uses the first prime and tablesize to get the size of hashtable
    * -h prints a help message describing how to use the program
    */
    while((option = getopt(argc, argv, optstring)) != EOF) {
      switch(option) {
      case 'c':
        if (optarg!=NULL) {
	  strcpy(text_filename, optarg);
	  printf("%s\n",text_filename);
	}
	c_flag=1;
	break;
      case 'd':
	hashtype = DOUBLE_H;
	break;
      case 'e':
	e_flag = 1;
	break;
      case 'p':
	if(c_flag==0){
	  p_flag = 1;
	}
	break;
      case 's':
	s_flag = 1;
	snapshots = atoi(optarg);
	break;
      case 't':
	table_size = next_prime(atoi(optarg));
	break;
      case 'h':
	h_flag = 1;
	break;
      default: 
	printf("Unknown input: -%c %s\n", option, optarg);
	return 1;
      }
    }

    h = htable_new(table_size, hashtype);
   
    /* This clock records the time it takes to fill table */
    start = clock();
    while(getword(word, sizeof(word), stdin) != EOF) {
        htable_insert(h, word);
        length++;
    
    }
    end = clock();
    fill_time  = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    
    if(e_flag == 1) {
      htable_print_entire_table(h);
      htable_print(h);
    }

    if(p_flag && s_flag) {
        htable_print_stats(h, stdout, snapshots);
    } else if(p_flag) {
        htable_print_stats(h, stdout, 10);
    } 
    
    if(c_flag==1) {
      
        FILE* file_pointer = fopen(text_filename, "r");
	
	if (file_pointer == NULL){
                printf("Cannot open file.\n");
                exit(EXIT_FAILURE);
	}
	
    /* This clock records the search time */
	start = clock();
        
        while (getword(word, sizeof word, file_pointer) != EOF){
                if (!htable_search(h,word)) {
                    printf("%s\n", word);
                    unknown++;
                } 
	}
        
        end = clock();
        search_time = ((double) (end - start)) / CLOCKS_PER_SEC;
	
        fprintf(stderr, "Fill time     : %f\n", fill_time);
        fprintf(stderr, "Search time   : %f\n", search_time);
        fprintf(stderr, "Unknown words = %d\n", unknown);
	fclose(file_pointer);
    }

    if(h_flag==1){
      printf(" Usage: ./sample-asgn [OPTION]... <STDIN>\n\n\
 Perform various operations using a hash table.  By default, words are\n \
 read from stdin and added to the hash table, before being printed out\n\
 alongside their frequencies to stdout.\n\n");
      printf(" -c FILENAME  Check spelling of words in FILENAME using \
words\n              from stdin as a dictionary.  Print any unknown words to\n\
              stdout, all timing info & count to stderr (ignores -p)\n\
 -d           Use double hashing (linear probing is the default)\n"
	     );
      printf(" -e           Display entire contents of hash table on \
stderr\n -p           Print stats info instead of frequencies & words\n\
 -s SNAPSHOTS Show SNAPSHOTS stats snapshots (if -p is used)\n\
 -t TABLESIZE Use the first prime >= TABLESIZE as the hashtable size\n\n\
 -h           Display a help message\n");
    }
    
    /* free the htable memory */
    htable_free(h);
    return EXIT_SUCCESS;
}
