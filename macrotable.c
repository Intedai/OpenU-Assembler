#include "macrotable.h"

macro_table_node* new_node(char *name, int beginning_cursor, int end_cursor) /*creates new macro table*/
{
    macro_table_node* temp = NULL;
    temp = (macro_table_node *) malloc(sizeof(macro_table_node));

    if(temp == NULL)/*check if memory allocation was successful*/
    {
        ALLOCATION_ERROR
        exit(1);
    }
    /*copy the name and set cursor positions*/
    temp->name = strdup(name);
    temp->beginning = beginning_cursor;
    temp->end = end_cursor;

    return temp;
}

void insert_to_end(macro_table_node** head, char *name, int beginning_cursor, int end_cursor)/*insert new node to the end of the list*/
{
    if(*head == NULL)/*if list is empty, insert to the head*/
        *head = new_node(name, beginning_cursor, end_cursor);
    else
    {
        macro_table_node *temp = *head;/*traverse the list to find the last node*/
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node(name, beginning_cursor, end_cursor);/*insert new node to the end*/
    }
}

macro_table_node* search_node(macro_table_node* head, char* macro_name)/*search for a macro_table_node with specified macro name*/
{
    while(head != NULL)/*traverse the list to find the matching node*/
    {
        if(strcmp(head->name,macro_name) == 0)
            return head;
        head = head->next;
    }
    return NULL;/*node is not found*/
}

void print_list(macro_table_node* head)/*prints the contents of the list*/
{
    macro_table_node* temp = head;
    while(temp != NULL)
    {
        printf("Name: \"%s\", Beginning: %d, End: %d\n", temp->name, temp->beginning, temp->end);
        temp = temp->next;
    }
}

void free_macro_table(macro_table_node* head)/*frees the memory allocated for thw macro_table_node*/
{
    macro_table_node* temp;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp->name);  
        free(temp);
    }
}
