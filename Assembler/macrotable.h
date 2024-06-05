#ifndef MACROTABLE_H
#define MACROTABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>

#include "utils.h"
#include "globals.h"

/* Struct representing a node in the macro table.*/
typedef struct macro_table_node {
    char *name;
    int beginning;
    int end;
    struct macro_table_node *next;
} macro_table_node;

/*
  Creates a new node for the macro table.
  @param name The name of the macro.
  @param beginning_cursor The beginning cursor position.
  @param end_cursor The end cursor position.
  @return Pointer to the newly created node.
*/
macro_table_node* new_node(char *name, int beginning_cursor, int end_cursor);

/*
  Inserts a node to the end of the macro table.
  @param head Pointer to the head of the macro table.
  @param name The name of the macro.
  @param beginning_cursor The beginning cursor position.
  @param end_cursor The end cursor position.
*/
void insert_to_end(macro_table_node** head, char *name, int beginning_cursor, int end_cursor);

/*
  Searches for a node in the macro table.
  @param head Pointer to the head of the macro table.
  @param macro_name The name of the macro to search for.
  @return Pointer to the found node, or NULL if not found.
*/
macro_table_node* search_node(macro_table_node* head, char* macro_name);


/*
  Prints the macro table list.
  @param head Pointer to the head of the macro table.
*/
void print_list(macro_table_node* head);

/*
  Frees the memory allocated for the macro table.
  @param head Pointer to the head of the macro table.
*/
void free_macro_table(macro_table_node* head);

#endif
