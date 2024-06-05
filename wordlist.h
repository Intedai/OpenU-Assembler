#ifndef WORDLIST_H
#define WORDLIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"

/*Structure representing a word with 14 bits.*/
typedef struct word {
    int word : 14;
} word;

/*Structure representing a node in a word list.*/
typedef struct word_list_node {
    word current_word;
    bool is_defined;
    struct word_list_node* next;
} word_list_node;

/*
  Creates a new word list node with the given word value and defined status.
  @param word_val The value of the word.
  @param defined Flag indicating if the word is defined.
  @return Pointer to the newly created word list node.
*/
word_list_node* new_word_node(int word_val,bool defined);

/*
  Inserts a new word to the end of the word list.
  @param head Pointer to the pointer to the head of the word list.
  @param word_val The value of the word to insert.
  @param defined Flag indicating if the word is defined.
*/
void insert_word_to_end(word_list_node** head, int word_val, bool defined);

/*
  Prints the word list.
  @param head Pointer to the head of the word list.
*/
void print_word_list(word_list_node* head);

void free_word_list(word_list_node* head);

/*
  Finds the first node that has is_defined = false.
  @param head Pointer to the head of the word list.
*/
word_list_node* find_first_undefined_node(word_list_node* head);

#endif
