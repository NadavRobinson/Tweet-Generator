#include "markov_chain.h"

/**
 * Get a random number between 0 and max_number (exclusive).
 * Uses modulo operation to generate a random number within the specified range.
 *
 * @param max_number Upper bound for random number generation
 * @return Randomly generated integer between 0 and max_number-1
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}

/**
 * Search for a node in the Markov Chain database with matching data.
 *
 * @param markov_chain Pointer to the Markov Chain structure
 * @param data_ptr String to search for in the database
 * @return Pointer to the found Node, or NULL if not found
 */
Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr){
    Node* temp = markov_chain->database->first;
    while(temp != NULL){
        // Compare the data strings to find a match
        if(strcmp(temp->data->data, data_ptr) == 0){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

/**
 * Add a new node to the Markov Chain database if it doesn't already exist.
 *
 * @param markov_chain Pointer to the Markov Chain structure
 * @param data_ptr String to be added to the database
 * @return Pointer to the added or existing Node, or NULL on failure
 */
Node* add_to_database(MarkovChain *markov_chain, char *data_ptr){
    // Check if node already exists to avoid duplicates
    Node* isExist = get_node_from_database(markov_chain, data_ptr);
    if (isExist)
        return isExist;

    // Allocate memory for a new MarkovNode
    MarkovNode* new_markov_node = malloc(sizeof(MarkovNode));
    if (!new_markov_node) {
        return NULL;
    }

    // Create a copy of the input string
    char *word = malloc(strlen(data_ptr) + 1);
    if (word == NULL) {
        free(new_markov_node);
        return NULL;
    }
    strcpy(word, data_ptr);

    // Initialize the MarkovNode with the new word
    *new_markov_node = (MarkovNode){word, NULL, 0};

    // Add to database, return NULL if addition fails
    if(add(markov_chain->database, new_markov_node) == 1)
        return NULL;
    return get_node_from_database(markov_chain, data_ptr);
}

/**
 * Add a node to the frequency list of another node, tracking word transitions.
 *
 * @param first_node Node from which the transition occurs
 * @param second_node Node to which the transition occurs
 * @return 0 on success, 1 on memory allocation failure
 */
int add_node_to_frequency_list(MarkovNode *first_node
        , MarkovNode *second_node){

    // Check if second_node is already in the frequency list
    for(int i = 0; i < first_node->size; i++){
        if(first_node->frequency_list[i].markov_node == second_node) {
            // If found, increment its frequency
            first_node->frequency_list[i].frequency++;
            return 0;
        }
    }

    // Reallocate memory to add a new frequency entry
    MarkovNodeFrequency* new_list = realloc(first_node->frequency_list, (first_node->size + 1) * sizeof(MarkovNodeFrequency));
    if(!new_list)
        return 1;

    first_node->frequency_list = new_list;

    // Add new frequency entry
    first_node->frequency_list[first_node->size].markov_node = second_node;
    first_node->frequency_list[first_node->size].frequency = 1;

    first_node->size++;

    return 0;
}

/**
 * Completely free all memory allocated for the Markov Chain.
 *
 * @param ptr_chain Double pointer to the Markov Chain to be freed
 */
void free_database(MarkovChain ** ptr_chain){
    // Handle case of empty database
    if((*ptr_chain)->database == NULL) {
        free(ptr_chain);
        ptr_chain = NULL;
        return;
    }

    Node* node = (*ptr_chain)->database->first;
    Node* next = NULL;

    // Iterate through all nodes and free their memory
    while (node != NULL) {
        next = node->next;  // Save next node before freeing current

        // Free frequency list
        free(node->data->frequency_list);
        node->data->frequency_list = NULL;

        // Free node data string
        free(node->data->data);
        node->data->data = NULL;

        // Free MarkovNode
        free(node->data);
        node->data = NULL;

        // Free the node itself
        free(node);
        node = next;
    }

    // Free database and chain
    free((*ptr_chain)->database);
    (*ptr_chain)->database = NULL;

    free(*ptr_chain);
    *ptr_chain = NULL;
}

/**
 * Select the first random node for tweet generation,
 * ensuring it doesn't end with a period.
 *
 * @param markov_chain Pointer to the Markov Chain
 * @return Pointer to a randomly selected MarkovNode
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain){
    Node *node;
    while (1){
        // Start at first node of database
        node = markov_chain->database->first;

        // Generate a random position in the database
        int rand_number = get_random_number(markov_chain->database->size);
        for(int i = 0; i < rand_number; i++){
            node = node->next;
        }

        // Ensure selected word doesn't end with a period
        char *word = node->data->data;
        if (word[strlen(word) - 1] != '.')
            return node->data;
    }
}

/**
 * Select the next random node based on frequency distribution.
 *
 * @param cur_markov_node Current MarkovNode
 * @return Pointer to the next randomly selected MarkovNode
 */
MarkovNode* get_next_random_node(MarkovNode *cur_markov_node){
    // Calculate total frequency of all possible next nodes
    int total_frequency = 0;
    for (int i = 0; i < cur_markov_node->size; i++){
        total_frequency += cur_markov_node->frequency_list[i].frequency;
    }

    // Generate a random number within the total frequency range
    int rand_number = get_random_number(total_frequency) + 1;

    // Select a node based on its relative frequency
    int count = 0;
    for (int i = 0; i < cur_markov_node->size; i++){
        count += cur_markov_node->frequency_list[i].frequency;
        if(rand_number <= count)
            return cur_markov_node->frequency_list[i].markov_node;
    }

    return NULL;
}

/**
 * Generate a tweet using Markov Chain algorithm.
 *
 * @param first_node Starting MarkovNode for tweet generation
 * @param max_length Maximum number of words in the generated tweet
 */
void generate_tweet(MarkovNode *first_node, int max_length){
    // Start with the first node
    MarkovNode *cur_node = first_node;

    // Print first word
    printf("%s", cur_node->data);

    int word_count = 1;

    // Generate tweet up to max_length words
    while (word_count < max_length){
        // Stop if current word ends with a period
        if (cur_node->data[strlen(cur_node->data) - 1] == '.')
            return;

        // Get next random node
        cur_node = get_next_random_node(cur_node);

        // Print next word
        printf(" %s", cur_node->data);

        word_count++;
    }
}