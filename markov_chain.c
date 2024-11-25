#include "markov_chain.h"

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}

Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr){
    Node* temp = markov_chain->database->first;
    while(temp != NULL){
        if(strcmp(temp->data->data, data_ptr) == 0){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

Node* add_to_database(MarkovChain *markov_chain, char *data_ptr){
    Node* isExist = get_node_from_database(markov_chain, data_ptr);
    if(isExist)
       return isExist;

    MarkovNode* new_markov_node = malloc(sizeof(MarkovNode));
    if (!new_markov_node) {
        return NULL;
    }

    char *word = malloc(strlen(data_ptr) + 1);
    if (word == NULL) {
        return NULL;
    }
    strcpy(word, data_ptr);

    // Initialize the MarkovNode
    *new_markov_node = (MarkovNode){word, NULL, 0};

    if(add(markov_chain->database, new_markov_node) == 1)
        return NULL;
    return get_node_from_database(markov_chain, data_ptr);
}

int add_node_to_frequency_list(MarkovNode *first_node
        , MarkovNode *second_node){

    // Check if second_node is already in the frequency_list
    for(int i = 0; i < first_node->size; i++){
        if(first_node->frequency_list[i].markov_node == second_node) {
            first_node->frequency_list[i].frequency++;
            return 0;
        }
    }

    // If not found, allocate memory for a new element
    MarkovNodeFrequency* new_list = realloc(first_node->frequency_list, (first_node->size + 1) * sizeof(MarkovNodeFrequency));
    if(!new_list)
        return 1;

    first_node->frequency_list = new_list;

    first_node->frequency_list[first_node->size].markov_node = second_node;
    first_node->frequency_list[first_node->size].frequency = 1;

    first_node->size++;

    return 0;
}

void free_database(MarkovChain ** ptr_chain){
    if((*ptr_chain)->database == NULL) {
        free(ptr_chain);
        ptr_chain = NULL;
        return;
    }
    Node* node = (*ptr_chain)->database->first;
    Node* next = NULL;

    while (node != NULL) {
        next = node->next;  // Save the next node before freeing the current one

        free(node->data->frequency_list);   // Free the frequency list in the node
        node->data->frequency_list = NULL;

        free(node->data->data);
        node->data->data = NULL;

        free(node->data);   // Free the data (Markov Node) in the node
        node->data = NULL;

        free(node);         // Free the node itself
        node = next;        // Move to the next node
    }

    free((*ptr_chain)->database);
    (*ptr_chain)->database = NULL;

    free(*ptr_chain);
    *ptr_chain = NULL;
}

MarkovNode* get_first_random_node(MarkovChain *markov_chain){
    Node *node;
    while (1){
        node = markov_chain->database->first;
        int rand_number = get_random_number(markov_chain->database->size);
        for(int i = 0; i < rand_number; i++){
            node = node->next;
        }
        char *word = node->data->data;
        if (word[strlen(word) - 1] != '.')
            return node->data;
    }
}

MarkovNode* get_next_random_node(MarkovNode *cur_markov_node){
    int total_frequency = 0;
    for (int i = 0; i < cur_markov_node->size; i++){
        total_frequency += cur_markov_node->frequency_list[i].frequency;
    }

    int rand_number = get_random_number(total_frequency) + 1;

    int count = 0;
    for (int i = 0; i < cur_markov_node->size; i++){
        count += cur_markov_node->frequency_list[i].frequency;
        if(rand_number <= count)
            return cur_markov_node->frequency_list[i].markov_node;
    }

    return NULL;
}

void generate_tweet(MarkovNode *first_node, int max_length){
    MarkovNode *cur_node = first_node;
    int word_count = 0;

    while (word_count < max_length){
        if (word_count > 0)
            printf(" ");

        printf("%s", cur_node->data);

        if (cur_node->data[strlen(cur_node->data) - 1] == '.')
            break;

        cur_node = get_next_random_node(cur_node);
        word_count++;
    }
}
