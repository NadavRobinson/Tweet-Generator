#include <stdlib.h>
#include <stdio.h>
#include "markov_chain.h"

#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"
#define SIZE_OF_LINE 1000

#define DELIMITERS " \n\t\r"

int fill_database(FILE* fp, int words_to_read, MarkovChain *markov_chain);

int main(int argc, char* argv[]){
    if(argc > 5 || argc < 4){
        printf("%s\n", NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }
    int seed = atoi(argv[1]);
    int numOfTweets = atoi(argv[2]);
    char *filename = argv[3];

    FILE* fp = fopen(filename, "r");
    if(!fp){
        printf("%s\n", FILE_PATH_ERROR);
        return EXIT_FAILURE;
    }

    int words_to_read = argc == 5 ? atoi(argv[4]) : -1;

    MarkovChain *markov_chain = malloc(sizeof(MarkovChain));
    if(!markov_chain) {
        fclose(fp);
        return EXIT_FAILURE;
    }

    markov_chain->database = malloc(sizeof(LinkedList));
    if(!markov_chain->database) {
        free_database(&markov_chain);
        fclose(fp);
        return EXIT_FAILURE;
    }

    *markov_chain->database = (LinkedList) {NULL, NULL, 0};

    if (fill_database(fp, words_to_read, markov_chain) == 1){
        free_database(&markov_chain);
        fclose(fp);
        return EXIT_FAILURE;
    }

    // Start generating tweets
    srand(seed);

    for (int i = 1; i <= numOfTweets; i++){
        MarkovNode *first_node = get_first_random_node(markov_chain);
        printf("Tweet %d: ", i);
        generate_tweet(first_node, 20);
        printf("\n");
    }

    free_database(&markov_chain);
    fclose(fp);
    return 0;
}

int fill_database(FILE* fp, int words_to_read, MarkovChain *markov_chain){
    char buffer[SIZE_OF_LINE];
    MarkovNode *prev_node = NULL;
    int word_count = 0;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {

        char* token = strtok(buffer, DELIMITERS);

        while (token != NULL) {
            MarkovNode *current_node = add_to_database(markov_chain, token)->data;
            if(current_node == NULL){
                return 1;
            }

            if(prev_node != NULL){
                if(add_node_to_frequency_list(prev_node, current_node) == 1)
                    return 1;
            }

            prev_node = token[strlen(token) - 1] == '.' ?  NULL : current_node;
            word_count++;

            if (words_to_read > 0 && word_count >= words_to_read) {
                return 0;
            }

            token = strtok(NULL, DELIMITERS);
        }

        prev_node = NULL;
    }
    return 0;
}






