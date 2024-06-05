#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "globals.h"
#include "utils.h"

/* Enumeration representing attributes for symbols.*/
typedef enum {
    mdefine,
    data,
    code,
    external
} Attribute;


/* Structure representing a node in the symbol table..*/
typedef struct symbol_table_node {
    char *symbol_name;
    Attribute attribute; /*the type of the symbol*/
    int value; /*the address or define value of the symbol*/
    bool is_entry; /*Flag indicating if the symbol is an entry*/
    struct symbol_table_node *next;
} symbol_table_node;

/*
  Creates a new symbol table node with the given attributes.
  @param name Name of the symbol.
  @param attr Attribute of the symbol.
  @param val Value associated with the symbol.
  @param is_entry Flag indicating if the symbol is an entry.
  @return Pointer to the newly created symbol table node.
*/
symbol_table_node* new_symbol_node(char *name, Attribute attr, int val, bool is_entry);

/*
  Inserts a new symbol to the end of the symbol table.
  @param head Pointer to the pointer to the head of the symbol table.
  @param name Name of the symbol to insert.
  @param attr Attribute of the symbol.
  @param val Value associated with the symbol.
  @param is_entry Flag indicating if the symbol is an entry.
  @param current_line Current line number for error reporting.
  @return True if the symbol was successfully inserted, false otherwise.
*/
bool insert_symbol_to_end(symbol_table_node** head, char *name, Attribute attr, int val, bool is_entry, int current_line);

/*
  Searches for a symbol node with the given name in the symbol table.
  @param head Pointer to the head of the symbol table.
  @param name Name of the symbol to search for.
  @return Pointer to the symbol node if found, NULL otherwise.
*/
symbol_table_node* search_symbol_node(symbol_table_node* head, char* name);

/*
  Frees the memory allocated for the symbol table.
  @param head Pointer to the head of the symbol table.
*/
void free_symbol_table(symbol_table_node* head);

/*
  Prints the symbol table.
  @param head Pointer to the head of the symbol table.
*/
void print_symbol_list(symbol_table_node* head);

/*
  Changes a symbol in the symbol table to an entry symbol.
  @param head Pointer to the head of the symbol table.
  @param name Name of the symbol to change to entry.
  @param current_line Current line number for error reporting.
  @return True if the symbol was successfully changed to entry, false otherwise.
*/
bool change_to_entry(symbol_table_node *head, const char *name, int current_line);

#endif
