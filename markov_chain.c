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
    *(new_markov_node) = (MarkovNode){data_ptr, NULL, 0};

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