#include "wordlist.h"

word_list_node* new_word_node(int word_val, bool defined) {
    /* Allocate memory for a node*/
    word_list_node* temp = (word_list_node *)malloc(sizeof(word_list_node));
    if (temp == NULL) {
        ALLOCATION_ERROR
        exit(1);
    }

    /* Set node values*/
    temp->current_word.word = word_val;
    temp->is_defined = defined;
    temp->next = NULL;  

    return temp;
}

void insert_word_to_end(word_list_node** head, int word_val, bool defined) {
    /* No nodes so make a new one */
    if (*head == NULL) {
        *head = new_word_node(word_val, defined);  
    } else {
        /*Go to end and insert node */
        word_list_node *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_word_node(word_val, defined);  
    }
}

word_list_node* find_first_undefined_node(word_list_node* head) {
    /* Iterate list until there's node that is undefined*/
    while (head != NULL) {
        if (!head->is_defined) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

void free_word_list(word_list_node* head)
{
    /* Iterate through list and free every node*/
    word_list_node* temp;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}

