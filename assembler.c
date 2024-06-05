#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "globals.h"
#include "preassembler.h"
#include "symboltable.h"
#include "wordlist.h"
#include "firstpass.h"
#include "secondpass.h"

int main(int argc, char *argv[]) {
    int i;
    int IC, DC;
    symbol_table_node* symbol_head;
    word_list_node* instruction_list;
    word_list_node* data_list;
    bool first_pass_bool;

    /* argc must be atleast 2: ./assembler and atleast 1 file*/
    if(argc < 2)
    {
        printf("./assembler: usage: ./assembler [files without including .as]\n");
        return 1;
    }

    for(i = 1; i < argc; ++i)
    {
        symbol_head = NULL;
        instruction_list = NULL;
        data_list = NULL;
        IC = 0;
        DC = 0;

        if(!file_exists(argv[i]))
            continue;

        if (expand_macros(argv[i])) 
        {
            printf("Macro expansion  of %s is successful.\n",argv[i]);
        } 
        else 
        {
            printf("Macro expansion of %s failed.\n",argv[i]);
            continue;
        }
        printf("Starting first pass:\n");
        first_pass_bool = first_pass(&symbol_head, argv[i], &instruction_list, &data_list,&IC, &DC);
        if(first_pass_bool)
        {
            printf("Second pass of %s is successful!\n",argv[i]);
            printf("Starting second pass:\n");
        }
        else
            printf("First pass of %s failed!\nChecking for second pass errors:\n",argv[i]);
        if(second_pass(&symbol_head, argv[i], &instruction_list, &data_list,&IC, &DC, !first_pass_bool))
        {
            printf("Second pass of %s is successful.\n",argv[i]);
        } 
        else 
        {
            printf("Second pass of %s failed.\n",argv[i]);
            continue;
        }        

        /* Reset lists for assembly of next file*/
        free_symbol_table(symbol_head);
        free_word_list(instruction_list);
        free_word_list(data_list);
        symbol_head = NULL;
        instruction_list = NULL;
        data_list = NULL;
    }
    return 0;
}
