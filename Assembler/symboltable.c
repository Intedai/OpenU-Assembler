#include "symboltable.h"

symbol_table_node* new_symbol_node(char *name, Attribute attr, int val, bool is_entry)
{
    symbol_table_node* temp = NULL;
    /* Allocate memory for new node*/
    temp = (symbol_table_node *) malloc(sizeof(symbol_table_node));

    if(temp == NULL)
    {
        ALLOCATION_ERROR
        exit(1);
    }

    /* Allocate memory for string and add every value*/
    temp->symbol_name = strdup(name);
    temp->attribute = attr;
    temp->value = val;
    temp->is_entry = is_entry;
    temp->next = NULL;

    return temp;
}

bool insert_symbol_to_end(symbol_table_node** head, char *name, Attribute attr, int val, bool is_entry, int current_line)
{
    bool return_bool = true;

    /* If list doesnt have any nodes make a new one has the head*/
    if(*head == NULL) {
        *head = new_symbol_node(name, attr, val, is_entry);
    } else {
        /*check if name is not in list and in the end insert it*/
        symbol_table_node *temp = *head;
        if (strcmp(temp->symbol_name, name) == 0) {
            return_bool = false;
        }
        while(temp->next != NULL) {
            temp = temp->next;
            if (strcmp(temp->symbol_name, name) == 0) {
                return_bool = false;
            }
        }
        /* Insert node*/
        temp->next = new_symbol_node(name, attr, val, is_entry);
    }
    if(!return_bool)
        print_error(ERROR_HEADER, "Symbol with the same name already exists", current_line);
    return return_bool;
}

/* Iterate through list */
symbol_table_node* search_symbol_node(symbol_table_node* head, char* name)
{
    while(head != NULL)
    {
        /* If found node with same name return it*/
        if(strcmp(head->symbol_name,name) == 0)
            return head;
        head = head->next;
    }
    return NULL;
}

bool change_to_entry(symbol_table_node *head, const char *name, int current_line) {
    symbol_table_node *current;

    current = head;

    while (current != NULL) {
        /* if found symbol with same name*/
        if (strcmp(current->symbol_name, name) == 0) {
            /* if symbol is of attribute data or code it can be entry, change it */
            if (current->attribute == data || current->attribute == code) {
                current->is_entry = true;
                break;
            }
            /* define cannot be entry*/
            else if(current->attribute == mdefine)
            {
                print_error(ERROR_HEADER,"Cannot put a define symbol in .entry", current_line);
                return false;
            }
            /* extern cannot be entry*/
            else if (current->attribute == external)
            {
                print_error(ERROR_HEADER,"Cannot have .entry label and .extern of the same name", current_line);
                return false;
            }
            
        }
        /* Move to the next node */
        current = current->next;
    }
    return true;
}

void print_symbol_list(symbol_table_node* head)
{
    symbol_table_node* temp = head;
    /* Iterate through list */
    while(temp != NULL)
    {
        /* Print name, attribute, value and is_entry in a formatted way*/
        printf("Name: \"%s\" Attribute: ",temp->symbol_name);
        switch (temp->attribute)
        {
        case mdefine:
            printf("mdefine ");
            break;
        case data:
            printf("data ");
            break;
        case code:
            printf("code ");
            break;
        case external:
            printf("external ");
            break;
        default:
            break;
        }
        printf("value: %d is_entry: %d\n",temp->value, (int)temp->is_entry);
        temp = temp->next;
    }
}

void free_symbol_table(symbol_table_node* head)
{
    symbol_table_node* temp;
    /* Iterate through list */
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        /* Free allocated string name*/
        free(temp->symbol_name);  
        /* Free node */
        free(temp);
    }
}

