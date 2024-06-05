#ifndef SECONDPASS_H
#define SECONDPASS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "globals.h"
#include "symboltable.h"
#include "wordlist.h"
#include "firstpass.h"

bool second_pass(symbol_table_node** symbol_head, char* file_name, word_list_node** instruction_list, word_list_node** data_list,int* IC, int* DC, bool first_pass_error);

#endif
