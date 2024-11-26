#include <stdlib.h>
#include <stdio.h>
#include "markov_chain.h"

// ** Constants for error messages and program parameters **
#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"
#define SIZE_OF_LINE 1000
#define DELIMITERS " \n\t\r"

int fill_database(FILE* fp, int words_to_read, MarkovChain *markov_chain);

// ** Main Function **
/**
 * Entry point of the program. Handles input arguments, initializes resources,
 * fills the Markov Chain database, and generates tweets.
 */
int main(int argc, char* argv[]) {
    // Check for the correct number of arguments
    if (argc > 5 || argc < 4) {
        printf("%s\n", NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }

    // Parse input arguments
    int seed = atoi(argv[1]);
    int numOfTweets = atoi(argv[2]);
    char *filename = argv[3];
    int words_to_read = (argc == 5) ? atoi(argv[4]) : -1;

    // Open the input file
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("%s\n", FILE_PATH_ERROR);
        return EXIT_FAILURE;
    }

    // Allocate and initialize the MarkovChain structure
    MarkovChain *markov_chain = malloc(sizeof(MarkovChain));
    if (!markov_chain) {
        fclose(fp);
        return EXIT_FAILURE;
    }

    markov_chain->database = malloc(sizeof(LinkedList));
    if (!markov_chain->database) {
        free_database(&markov_chain);
        fclose(fp);
        return EXIT_FAILURE;
    }
    *markov_chain->database = (LinkedList){NULL, NULL, 0};

    // Populate the Markov Chain database
    if (fill_database(fp, words_to_read, markov_chain) == 1) {
        free_database(&markov_chain);
        fclose(fp);
        return EXIT_FAILURE;
    }

    // Seed the random number generator and generate tweets
    srand(seed);
    for (int i = 1; i <= numOfTweets; i++) {
        MarkovNode *first_node = get_first_random_node(markov_chain);
        printf("Tweet %d: ", i);
        generate_tweet(first_node, 20);
        printf("\n");
    }

    // Clean up and free resources
    free_database(&markov_chain);
    fclose(fp);
    return 0;
}

// ** Helper Function Implementations **

/**
 * fill_database:
 * Reads words from the file, processes them into the Markov Chain database,
 * and builds frequency links between nodes.
 *
 * @param fp - Pointer to the opened file.
 * @param words_to_read - Maximum number of words to read; -1 for unlimited.
 * @param markov_chain - Pointer to the Markov Chain structure.
 *
 * @return 0 on success, 1 on failure.
 */
int fill_database(FILE* fp, int words_to_read, MarkovChain *markov_chain) {
    char buffer[SIZE_OF_LINE];
    MarkovNode *prev_node = NULL;
    int word_count = 0;

    // Read lines from the file
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Tokenize the line into words
        char* token = strtok(buffer, DELIMITERS);
        while (token != NULL) {
            // Add token to the database and retrieve its node
            MarkovNode *current_node = add_to_database(markov_chain, token)->data;
            if (current_node == NULL) {
                return 1;  // Memory or allocation failure
            }

            // Link the current node to the previous node in the frequency list
            if (prev_node != NULL) {
                if (add_node_to_frequency_list(prev_node, current_node) == 1) {
                    return 1;
                }
            }

            // Determine if the current word ends the sentence
            prev_node = (token[strlen(token) - 1] == '.') ? NULL : current_node;
            word_count++;

            // Stop if the word limit is reached
            if (words_to_read > 0 && word_count >= words_to_read) {
                return 0;
            }

            // Move to the next token
            token = strtok(NULL, DELIMITERS);
        }
        prev_node = NULL;  // Reset at the end of each line
    }
    return 0;
}
