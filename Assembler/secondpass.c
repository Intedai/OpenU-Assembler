#include "secondpass.h"

bool second_pass_error = false;
int secondpass_current_line = 0;

/*
  Count the amount of external symbols
  @param head Pointer to the head of the symbol table.
  @return number of externals.
*/
int count_externals(symbol_table_node *head) {
    int count = 0;
    symbol_table_node *current = head;

    while (current != NULL) {
        if (current->attribute == external) {
            count++;
        }
        current = current->next;
    }
    return count;
}

/*
  Count the amount of entry symbols
  @param head Pointer to the head of the symbol table.
  @return number of nodes with is_entry as true.
*/
int count_entry_true(symbol_table_node *head) {
    int count = 0;
    symbol_table_node *current = head;
    while (current != NULL) {
        if (current->is_entry == true) {
            count++;
        }
        current = current->next;
    }
    return count;
}

/*
  Calculate the amount of words generated from an instruction according to the shit
  @param inst Pointer to the instruction node
  @return number amount of words generated from an instruction.
*/
int calculate_L(Instruction* inst)
{
    int L = WORD;

    if(inst->first_operand_type == Reg&& inst->second_operand_type == Reg)
        return TWO_WORDS;

    if(inst->first_operand_type == Reg || inst->first_operand_type == Direct || inst->first_operand_type == Immediate)
    {
        L+=WORD;
    }
    else if (inst->first_operand_type == Index)
    {
        L+=TWO_WORDS;
    }    
    if(inst->second_operand_type == Reg || inst->second_operand_type == Direct || inst->second_operand_type == Immediate)
    {
        L+=WORD;
    }
    else if (inst->second_operand_type == Index)
    {
        L+=TWO_WORDS;
    }
    return L;
}

/*
  Calculate the amount of words generated from an instruction according to the shit
  @param line current line to extract from
  @param head Double Pointer to symbol table
  @return True if no errors
*/
bool extract_entry(char *line, symbol_table_node **head)
{
    char* new_line = trimString(line);
    char *entry_string = strstr(new_line, entry_dir);
    char *token;

    /* '.entry' not found, no data to process. */
    if (entry_string == NULL) {
        free(new_line);
        return false; 
    }

    entry_string += strlen(entry_dir);    

    token = strtok(entry_string, WHITESPACES);

    if(token == NULL)
    {
        print_error(ERROR_HEADER,"No label after .entry definition\n",secondpass_current_line);
        free(new_line);
        return false;
    }
    while(token != NULL)
    {
        if(!is_legal_label_name(token,secondpass_current_line, false))
        {
            print_error(ERROR_HEADER,"Label defined at .entry is not written according to the label rules\n",secondpass_current_line);
            free(new_line);
            return false;
        }
        if(!change_to_entry(*head,token,secondpass_current_line))
        {
            free(new_line);
            return false;
        }
        token = strtok(NULL, WHITESPACES);
    }
    free(new_line);
    return true;
}

/*
  Create entries file
  @param char pointer file name
  @param symbol_head Pointer to symbol table
*/
void make_entries_file(char* file_name, symbol_table_node* symbol_head)
{
    FILE* output_file;
    symbol_table_node *current;

    char *output_file_name = malloc(sizeof(char) * (strlen(file_name) + strlen(entries_extension) + 1));
    if(output_file_name == NULL)
    {
        ALLOCATION_ERROR
        exit(1);        
    }

    strcpy(output_file_name, file_name);
    output_file_name = strcat(output_file_name, entries_extension);

    output_file = fopen(output_file_name, "w");

    if(output_file == NULL)
    {
        FILE_OPEN_ERROR
        exit(1);
    }
    
    current = symbol_head;
    while (current != NULL) {
        if (current->is_entry) {
            fprintf(output_file, "%s\t%04d\n", current->symbol_name, current->value);
        }
        current = current->next;
    }

    fclose(output_file);
    free(output_file_name);
}

/*
  Helper function for .ob generator 
  @param word_list_node pointer word to generate from in base 4
  @param int address current address to put in file
  @param char pointer line_buffer just a line buffer
  @param int pointer symbol_count symbol count
*/
void process_list(word_list_node* list, FILE* output_file, int* address, char* line_buffer, int* symbol_count) {
    word_list_node *current = list;
    int base4_digit, i;
    char symbol;

    while (current != NULL) {
        *symbol_count = 0;
        for (i = 12; i >= 0; i -= 2) {
            base4_digit = (current->current_word.word >> i) & 3;
            switch (base4_digit) {
                case 0: symbol = '*'; break;
                case 1: symbol = '#'; break;
                case 2: symbol = '%'; break;
                case 3: symbol = '!'; break;
            }
            line_buffer[(*symbol_count)++] = symbol;
            if (*symbol_count == 7) {
                line_buffer[*symbol_count] = '\0'; /* Null-terminate the string */
                fprintf(output_file, "%04d %s\n", (*address)++, line_buffer);
                *symbol_count = 0; /* Reset count for the next line */
            }
        }
        current = current->next;
    }
    if (*symbol_count > 0) { /* Write any remaining symbols that did not fill a line */
        line_buffer[*symbol_count] = '\0';
        fprintf(output_file, "%04d %s\n", (*address)++, line_buffer);
    }
}

/*
  generate .ob file 
  @param char* file_name
  @param word_list_node* instruction_list first word list to put in file
  @param word_list_node* data_list second word to put in file
  @param int IC for start of file
  @param int DC for start of file
*/
void make_ob_file(char* file_name, word_list_node* instruction_list, word_list_node* data_list, int IC, int DC) {
    FILE* output_file;
    int address = MEMORY_START;
    char line_buffer[8]; /* Buffer to hold up to 7 symbols plus a null terminator */
    int symbol_count = 0; /* Count symbols to manage line breaks */

    char *output_file_name = malloc(sizeof(char) * (strlen(file_name) + strlen(object_extension) + 1));
    if (output_file_name == NULL) {
        ALLOCATION_ERROR;
        exit(1);
    }

    strcpy(output_file_name, file_name);
    strcat(output_file_name, object_extension);

    output_file = fopen(output_file_name, "w");
    if (output_file == NULL) {
        FILE_OPEN_ERROR;
        free(output_file_name);
        exit(1);
    }

    fprintf(output_file, "\t%d\t%d\n", IC, DC); /* Write IC and DC at the top of the file */

    /* Process instruction list */
    process_list(instruction_list, output_file, &address, line_buffer, &symbol_count);
    /* Process data list */
    process_list(data_list, output_file, &address, line_buffer, &symbol_count);

    fclose(output_file);
    free(output_file_name);
}

/*
  generate .ob file 
  @param char* file_name
  @return get file pointer for externals to add lines to while in main second pass func
*/
FILE* make_externals_file(char* file_name)
{
    FILE* output_file;
    char* output_file_name = malloc(sizeof(char) * (strlen(file_name) + strlen(externals_extension) + 1));
    if(output_file_name == NULL)
    {
        ALLOCATION_ERROR;
        exit(1);        
    }

    strcpy(output_file_name, file_name);
    strcat(output_file_name, externals_extension);

    output_file = fopen(output_file_name, "w");
    free(output_file_name);

    if(output_file == NULL)
    {
        FILE_OPEN_ERROR;
        exit(1);
    }

    return output_file;
}

bool second_pass(symbol_table_node** symbol_head, char* file_name, word_list_node** instruction_list, word_list_node** data_list,int* IC, int* DC, bool first_pass_error)
{
    FILE* preassembled_file;
    FILE* externals_file = make_externals_file(file_name);
    char* preassembled_file_name;
    char* label_name;
    char* line = malloc(sizeof(char) * LINE_SIZE);
    Instruction *current_instruction = (Instruction *) malloc(sizeof(Instruction));
    word_list_node* current_empty_code = NULL;
    char* external_filename = malloc(sizeof(char) * (strlen(file_name) + strlen(externals_extension) + 1));

    symbol_table_node* temp_symbol;

    char* word_label_name;
    char* temp;
    /* If it's a label*/
    bool is_symbol = false;

    preassembled_file_name = malloc(sizeof(char) * (strlen(file_name) + strlen(macro_extension) + 1));

    second_pass_error = first_pass_error;
    secondpass_current_line = 0;

    if(preassembled_file_name == NULL || external_filename == NULL)
    {
        ALLOCATION_ERROR
        exit(1);        
    }

    strcpy(external_filename, file_name);
    external_filename =strcat(external_filename, externals_extension);
    strcpy(preassembled_file_name, file_name);
    preassembled_file_name = strcat(preassembled_file_name, macro_extension);

    preassembled_file = fopen(preassembled_file_name, "r");
    if(preassembled_file == NULL)
    {
        FILE_OPEN_ERROR
        exit(1);
    }

    *IC = 0;

    while (fgets(line, LINE_SIZE, preassembled_file)){
        ++secondpass_current_line;
        /*check whitespaces and commets*/
        if(is_empty_line(line) || is_comment_line(line))
            continue;

        /* If symbol, skip*/
        is_symbol = is_label(line, &label_name);
        if (is_directive(line,data_dir,is_symbol) ||
            is_directive(line,string_dir,is_symbol) ||
            is_directive(line,extern_dir,is_symbol) ||
            is_directive(line,define_dir,is_symbol))
        {
            continue;
        }
        /* if entry extract*/
        else if (is_directive(line,entry_dir,is_symbol)) {
            if(!extract_entry(line,symbol_head))
            {
                second_pass_error = true;
            }
        }

        else 
        {
            /* Parse instruction again and fill word lists, put external in file*/
            if(parse_instruction(line, current_instruction, is_symbol,secondpass_current_line, false)) { 
                
                /* if Direct just use operand name and get from list*/
                if(current_instruction->first_operand_type == Direct)
                {
                    current_empty_code = find_first_undefined_node(*instruction_list);
                    current_empty_code->is_defined = true;

                    temp_symbol = search_symbol_node(*symbol_head, current_instruction->first_operand);
                    if(temp_symbol == NULL)
                    {
                        second_pass_error = true;
                    }
                    else
                    {
                        current_empty_code->current_word.word = (temp_symbol->value) << 2;
                        if(temp_symbol->attribute != external)
                            current_empty_code->current_word.word |= ARE_R;
                        else
                        {
                            current_empty_code->current_word.word |= ARE_E;
                            /* put in externals file */
                            fprintf(externals_file, "%s\t%04d\n", temp_symbol->symbol_name, *IC + MEMORY_START + 1);
                        }
                    }
                }
                /* if Index extract label name from label[index] and get from list*/
                else if (current_instruction->first_operand_type == Index)
                {
                    current_empty_code = find_first_undefined_node(*instruction_list);
                    current_empty_code->is_defined = true;

                    word_label_name = strdup(current_instruction->first_operand);

                    temp = strchr(word_label_name, '[');
                    if (temp) {
                        *temp = '\0';
                    }
                    temp_symbol = search_symbol_node(*symbol_head, word_label_name);
                    if(temp_symbol == NULL)
                    {
                        second_pass_error = true;
                    }
                    else
                    {
                        current_empty_code->current_word.word = (temp_symbol->value) << 2;
                        if(temp_symbol->attribute != external)
                            current_empty_code->current_word.word |= ARE_R;
                        else
                        {
                            current_empty_code->current_word.word |= ARE_E;
                            /* put in externals file */
                            fprintf(externals_file, "%s\t%04d\n", temp_symbol->symbol_name, *IC + MEMORY_START + 1);
                        }
                        
                    }

                    free(word_label_name);
                } 

                /* Second operand: same as the first operand*/
                if(current_instruction->second_operand_type == Direct)
                {
                    current_empty_code = find_first_undefined_node(*instruction_list);
                    current_empty_code->is_defined = true;

                    temp_symbol = search_symbol_node(*symbol_head, current_instruction->second_operand);
                    if(temp_symbol == NULL)
                    {
                        second_pass_error = true;
                    }
                    else
                    {
                        current_empty_code->current_word.word = (temp_symbol->value) << 2;
                        if(temp_symbol->attribute != external)
                            current_empty_code->current_word.word |= ARE_R;
                        else
                        {
                            current_empty_code->current_word.word |= ARE_E;
                            fprintf(externals_file, "%s\t%04d\n", temp_symbol->symbol_name, *IC + MEMORY_START + 1);
                        }
                    }
                }
                else if (current_instruction->second_operand_type == Index)
                {
                    current_empty_code = find_first_undefined_node(*instruction_list);
                    current_empty_code->is_defined = true;

                    word_label_name = strdup(current_instruction->second_operand);

                    temp = strchr(word_label_name, '[');
                    if (temp) {
                        *temp = '\0';
                    }
                    temp_symbol = search_symbol_node(*symbol_head, word_label_name);
                    if(temp_symbol == NULL)
                    {
                        second_pass_error = true;
                    }
                    else
                    {
                        current_empty_code->current_word.word = (temp_symbol->value) << 2;
                        if(temp_symbol->attribute != external)
                            current_empty_code->current_word.word |= ARE_R;
                        else
                        {
                            current_empty_code->current_word.word |= ARE_E;
                            fprintf(externals_file, "%s\t%04d\n", temp_symbol->symbol_name, *IC + MEMORY_START + 1);
                        }
                    }

                    free(word_label_name);
                }              
            }
            /*skip for next instruction*/
            *IC += calculate_L(current_instruction);     

            /* Free allocated memory for efficiency*/     
            if(current_instruction->first_operand != NULL)
            {
                free(current_instruction->first_operand);
                current_instruction->first_operand = NULL;
            }
            if(current_instruction->second_operand != NULL)
            {
                free(current_instruction->second_operand);
                current_instruction->second_operand = NULL;
            }
        }
    }

    fclose(preassembled_file);
    fclose(externals_file);

    if(!second_pass_error)
    {
        if(count_entry_true(*symbol_head) > 0)
            make_entries_file(file_name,*symbol_head);
        if(count_externals(*symbol_head) == 0)
        {
            if (remove(external_filename) != 0)
            {
                FILE_REMOVE_ERROR
                exit(1);
            }
        }
        /* Make object file*/
        make_ob_file(file_name, *instruction_list, *data_list, *IC, *DC);
    }
    else
    {
        if (remove(external_filename) != 0)
        {
            FILE_REMOVE_ERROR
            exit(1);
        }
    }

    free(preassembled_file_name);
    free(external_filename);
    free(current_instruction);
    return !second_pass_error;
}
