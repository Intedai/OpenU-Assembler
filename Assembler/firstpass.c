#include "firstpass.h"

bool first_pass_error = false;
int firstpass_current_line = 0;

/*
  Extract from define line
  @param char *line
  @param symbol_table_node **symbol_table
  @returns True if successful
*/
bool extract_define(char *line, symbol_table_node **symbol_table) {
    char *new_line;
    char *define_string;
    char *token;
    char *variable_name;
    char *value_string;
    long value_long;
    char *temp_endptr;

    /* Trim string and skip to .define */
    new_line = trimString(line);
    define_string = strstr(new_line, define_dir);

    /* If there's no .define raise error */
    if (define_string == NULL) {
        print_error(ERROR_HEADER,"String does not include .define!",firstpass_current_line);
        free(new_line);
        return false;
    }

    /* Skip .define*/
    define_string += strlen(define_dir);

    /* If there's more than 1 equal sign raise error */
    if(count_char_in_string(new_line,'=') > 1)
    {
        print_error(ERROR_HEADER,"More than 1 '='!",firstpass_current_line);
        free(new_line);
        return false;
    }

    /* Get .define variable name */
    token = strtok(define_string,"=");
    if (token == NULL) {
        print_error(ERROR_HEADER,"No '=' after .define!",firstpass_current_line);
        free(new_line);
        return false;
    }

    /* Allocate a trimmed string for the variable name */
    variable_name = trimString(token);

    /* Check if legal name for the variable if not raise an error */
    if (!(is_legal_label_name(variable_name,firstpass_current_line, true))) {
        free(variable_name);
        free(new_line);
        return false;
    }

    /* Get value after equal sign*/
    token = strtok(NULL,WHITESPACES"=");

    /* If theres no value after equal sign raise error*/
    if (token == NULL) {
        print_error(ERROR_HEADER,"No value in .define after '='!",firstpass_current_line);
        free(variable_name);
        free(new_line);
        return false;
    }

    /* Allocate a trimmed string for the variable value */
    value_string = trimString(token);

    value_long = strtol(value_string, &temp_endptr, 10);

    if(strtok(NULL,WHITESPACES) != NULL)
    {
        print_error(ERROR_HEADER,"There shouldn't be more text in define line after the value!",firstpass_current_line);
        free(variable_name);
        free(value_string);
        free(new_line);
        return false;
    }

    /* Check if only number and not extra string*/
    if (*temp_endptr == '\0' && temp_endptr != value_string) {
        if(!insert_symbol_to_end(symbol_table,variable_name,mdefine,(int)value_long,false,firstpass_current_line))
        {
            free(variable_name);
            free(value_string);
            free(new_line);
            return false;
        }
    }
    else /* Not a number */
    {
        print_error(ERROR_HEADER,"Value in a define line must be a number!",firstpass_current_line);
        free(variable_name);
        free(value_string);
        free(new_line);
        return false;
    }
    /* passed every check it extracted successfully */
    free(value_string);
    free(variable_name);
    free(new_line);
    return true;
}

/*
  Extract from data line
  @param char *line
  @param word_list_node** data_list
  @param int *DC, symbol_table_node
  @param symbol_table_node *symbol_table
  @returns True if successful
*/
bool extract_data(char *line, word_list_node** data_list, int *DC, symbol_table_node *symbol_table)
{
    symbol_table_node *temp_node;
    long temp_value;
    char *temp_endptr;
    bool contains_error = false;
    char *new_line = trimString(line);
    char *data_string = strstr(new_line, data_dir);
    char *token = NULL;

    /* '.data' not found, no data to process. */
    if (data_string == NULL) {
        print_error(ERROR_HEADER,"String does not include .data!",firstpass_current_line);
        free(new_line);
        return false; 
    }

    /* Skip .data text to go to the data itself*/
    data_string += strlen(data_dir);

    /**/
    if(!verify_commas(data_string,firstpass_current_line))
    {
        free(new_line);
        return false; 
    }


    token = strtok(data_string, WHITESPACES",");

    if (token == NULL) {
        print_error(ERROR_HEADER,"Data is empty",firstpass_current_line);
        free(new_line);
        return false;
    }

    while(token != NULL){
        temp_endptr = NULL;

        if((temp_node = search_symbol_node(symbol_table, token)) != NULL)
        {
            if(temp_node->attribute == mdefine)
            {
                if (temp_node->value > MAX_12_BITS || temp_node->value < MIN_12_BITS)
                {
                    print_error(ERROR_HEADER,"Data number must be 12 bits even though word is 14 bits!",firstpass_current_line);
                    contains_error = true;
                }
                else
                {
                    insert_word_to_end(data_list, temp_node->value, true);
                    (*DC)++;
                }
            }
            else
            {
                print_error(ERROR_HEADER,"Symbol that is not define variable was used",firstpass_current_line);
                contains_error = true;
            }
        }
        else
        {
            temp_value = strtol(token, &temp_endptr, 10);
            /* Check if only number and not extra string*/
            

            if (*temp_endptr == '\0' && temp_endptr != token) {
                /*number checks that its 12 bit by max min number*/
                if (temp_value > MAX_12_BITS || temp_value < MIN_12_BITS)
                {
                    print_error(ERROR_HEADER,"Data number must be 12 bits even though word is 14 bits!",firstpass_current_line);
                    contains_error = true;
                }
                else
                {
                    insert_word_to_end(data_list, temp_value, true);
                    (*DC)++;
                }
            }
            else
            {
                /* BAD DATA MAKE ERROR*/
                contains_error = true;
                break;
            }
        }
        token = strtok(NULL, WHITESPACES",");
    }
   
    free(new_line);
    return !contains_error;
}

/*
  Extract from string line
  @param char *line
  @param word_list_node** data_list
  @param int *DC, symbol_table_node
  @returns True if successful
*/
bool extract_string(char *line, word_list_node** data_list, int *DC)
{
    char *new_line = trimString(line);
    char *data_string = strstr(new_line, string_dir);

    /* '.string' not found, no data to process. */
    if (data_string == NULL) {
        print_error(ERROR_HEADER,"String does not include .string!",firstpass_current_line);
        free(new_line);
        return false; 
    }

    data_string += strlen(string_dir);
    while (isspace(*data_string))
        ++data_string;
    
    /* If string doesn't start with a quote mark*/
    if (*data_string != '"') {
        print_error(ERROR_HEADER,"String does not start with a quote mark!",firstpass_current_line);
        free(new_line);
        return false; 
    }

    /* Move past the opening quote. */
    ++data_string;
    
    /* Inserting chars into the list that contains data words*/
    while (*data_string != '\0' && *data_string != '"') {
        insert_word_to_end(data_list, *data_string, true);
        (*DC)++;
        ++data_string;
    }
    /* If reached end of string successfully insert NULL terminating character*/
    if (*data_string == '"') 
    {
        insert_word_to_end(data_list, '\0', true);
        (*DC)++;
        data_string++; /* Move past the closing quote. */
    }
    /* If string doesn't end with quote mark*/
    else
    {
        print_error(ERROR_HEADER,"String does not end with a quote mark!",firstpass_current_line);
        free(new_line);
        return false; 
    }

    while (isspace(*data_string))
        ++data_string;

    /* Check if there's more text after string definition if there is raise an error. */
    if (*data_string != '\0') {
        print_error(ERROR_HEADER,"Additional text found after string definition\n",firstpass_current_line);
        free(new_line);
        return false; 
    }

    free(new_line);

    /* Return true if no errors occurred. */
    return true; 
}

/*
  Extract from extern line
  @param char *line
  @param symbol_table_node *symbol_table
  @returns True if successful
*/
bool extract_extern(char *line, symbol_table_node **head)
{
    char* new_line = trimString(line);
    char *extern_string = strstr(new_line, extern_dir);
    char *token;

    /* '.extern' not found, no data to process. */
    if (extern_string == NULL) {
        free(new_line);
        return false; 
    }

    extern_string += strlen(extern_dir);    

    token = strtok(extern_string, WHITESPACES);

    if(token == NULL)
    {
        print_error(ERROR_HEADER,"No label after .extern definition\n",firstpass_current_line);
        free(new_line);
        return false;
    }
    while(token != NULL)
    {
        if(!is_legal_label_name(token,firstpass_current_line, true))
        {
            free(new_line);
            return false;
        }
        insert_symbol_to_end(head,token,external,0,false,firstpass_current_line);
        token = strtok(NULL, WHITESPACES);
    }
    free(new_line);
    return true;
}

/*
  Get operand count of instruction from index
  @param int index
  @returns the amount of params
*/
int get_operand_count(int index)
{
    /* the insts are in row from 2 params to 1 to 0 so its just index range*/
    if (index <= 4)
    {
        return 2;
    }
    else if (index <= 13)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
  get_instruction_index
  @param char *line
  @param bool contains_label
  @returns index
*/
int get_instruction_index(char *line, bool contains_label)
{
    const char* instructions[] = INSTRUCTIONS;
    char *new_line = trimString(line);
    char *temp = new_line;
    char *extracted_instruction = NULL;
    int i;
    if(contains_label)
    {
        while(*temp != ':')
        {
            ++temp;
        }
        ++temp;
    }
    
    extracted_instruction = strtok(temp, WHITESPACES);
    
    if(extracted_instruction == NULL){ /*makes sure the token is valid*/
        free(new_line);
        return -1;
    }

    for(i = 0; i < INSTRUCTION_COUNT; ++i)
    {
        if(strcmp(extracted_instruction,instructions[i]) == 0)
        {
            return i;
        }
    }
    free(new_line);
    return -1;
}

/* returns register from operand*/
Register get_register(char *operand)
{
    int register_number;

    if(strlen(operand) != 2 || operand[0] != 'r')
        return NOT_REG;

    if(operand[1] < '0' || operand[1] > '7')
        return NOT_REG;

    register_number = operand[1] - '0';

    if(register_number >= 0 && register_number <= 7)
        return (Register)register_number;

    return NOT_REG;
}

/* returns immedieate data from operand*/
bool get_immediate_data(char *operand, symbol_table_node *symbol_table, int *data)
{
    long temp_value = 0;
    char *temp_endptr;
    symbol_table_node *temp_node;
    char *new_operand = strdup(operand);
    char *temp;

    if(new_operand[0] != '#')
    {
        free(new_operand);
        return false;
    }

    temp = new_operand;

    /*skin the #*/
    ++temp;

    if((temp_node = search_symbol_node(symbol_table, temp)) != NULL)
    {
        if(temp_node->attribute == mdefine)
            *data = temp_node->value;
        else
        {
            print_error(ERROR_HEADER,"symbol is not a define symbol\n",firstpass_current_line);
            free(new_operand);
            return false;   
        }
    }
    else{
            temp_value = strtol(temp, &temp_endptr, 10);
            /* Check if only number and not extra string*/
            if (*temp_endptr == '\0' && temp_endptr != new_operand) {
                /*number checks that its 14 bit by max min number*/
                *data = (int)temp_value;
            }
            else
            {
                /*exit(21) false name*/
                free(new_operand);
                return false;
            }

    }
    free(new_operand);
    return true;
}

/* checks if index addressing*/
bool is_index_addr(char *operand) {
    char *new_operand = strdup(operand);
    char *temp = new_operand;

    /** Check if square bracket start exists */
    while (*temp != '[' && *temp != '\0')
        ++temp;
    if (*temp == '\0') {
        free(new_operand);
        return false;
    }

    /** Move past the '[' for further checking */
    temp++;

    /** Check if square bracket end exists after '[' */
    while (*temp != ']' && *temp != '\0')
        ++temp;
    if (*temp == '\0') {
        free(new_operand);
        return false;
    }

    /** Move past the ']' to check for trailing characters */
    temp++;

    /** Skip any whitespace after the closing bracket */
    while (isspace(*temp))
        ++temp;

    /** If any non-space characters remain, return false */
    if (*temp != '\0') {
        free(new_operand);
        return false;
    }

    /** Cut the square brackets out of the string for label checking */
    temp = new_operand;
    while (*temp != '[')
        ++temp;
    *temp = '\0';  /** Terminate string at the first '[' */

    /** Check if the initial part of the operand is a legal label name */
    if (is_legal_label_name(new_operand,firstpass_current_line, false)) {
        free(new_operand);
        return true;
    } else {
        free(new_operand);
        return false;        
    }
}

/* returns address method */
AddrMethodsOptions get_addr_method(char *operand)
{
    if(get_register(operand) != NOT_REG)
        return Reg;
    else if (*operand == '#')
    {
        return Immediate;
    }
    else if (is_index_addr(operand))
    {
        return Index;
    }
    else if (is_legal_label_name(operand,firstpass_current_line, false))
    {

        return Direct;
    }
    return NO_METHOD;
}

/* return array index from index addressing method */
bool get_array_index(char *operand, symbol_table_node* symbol_table, int* data)
{
    char *new_operand = strdup(operand);
    char *token;
    int closing_bracket_index;
    symbol_table_node *temp_node;
    long value_long;
    char *temp_endptr;
    token = strtok(new_operand, "[");
    token = strtok(NULL, "[");

    closing_bracket_index = strcspn(token, "]");
    token[closing_bracket_index] = '\0';


    value_long = strtol(token, &temp_endptr, 10);
    /* Check if only number and not extra string*/
    if (*temp_endptr == '\0' && temp_endptr != token) {
        *data = (int)value_long;
        free(new_operand);
        return true;
    }
    
    if(is_legal_label_name(token,firstpass_current_line, false))
    {
        if((temp_node = search_symbol_node(symbol_table, token)) != NULL)
        {
            if(temp_node->attribute == mdefine)
            {
                *data = temp_node->value;
                free(new_operand);
                return true;
            }
            else
            {
                printf("ERROR: Do not use a label as the index of define array in line %d",firstpass_current_line);
                free(new_operand);
                return false;
            }
        }
    }

    printf("ERROR: Index must include define variable or be a number in line %d",firstpass_current_line);
    free(new_operand);
    return false;

}

/* get code for building word from index of instruction */
int get_instruction_code(int instruction_index)
{
    switch (instruction_index)
    {
    case MOV:
        return 0;
    case CMP:
        return 1;
    case ADD:
        return 2;
    case SUB:
        return 3;
    case NOT:
        return 4;
    case CLR:
        return 5;
    case LEA:
        return 6;
    case INC:
        return 7;
    case DEC:
        return 8;
    case JMP:
        return 9;
    case BNE:
        return 10;
    case RED:
        return 11;
    case PRN:
        return 12;
    case JSR:
        return 13;
    case RTS:
        return 14;
    case HLT:
        return 15;
    default:
        break;
    }
    return UNDEFINED; /*unkown index*/
}


bool parse_instruction(char* line, Instruction *current_instruction, bool contains_label, int current_line, bool print_errors)
{
    char *new_line = trimString(line);
    char *temp = new_line;
    char *comma_line = NULL;
    char *comma_temp = NULL;
    int instruction_index = get_instruction_index(line, contains_label);
    char *first_operand_token = NULL;
    char *second_operand_token = NULL;
    int current_operand_count;

    if(instruction_index == -1)
    {
        print_error(ERROR_HEADER, "Instruction does not exist!", current_line);
        free(new_line);
        return false;
    }
    if(contains_label)
    {
        while(*temp != ':')
        {
            ++temp;
        }
        ++temp;
    }

    current_operand_count = get_operand_count(instruction_index);    
    if(current_operand_count == 2)
    {
        comma_line = trimString(temp);
        comma_temp = comma_line;
        while(!isspace(*comma_temp) && *comma_temp != '\0')
            ++comma_temp;
    }
    
    /*Skip instruction*/
    strtok(temp,WHITESPACES);
    
    if(current_operand_count == 2)
    {
        if(!verify_commas(comma_temp,firstpass_current_line))
        {
            free(comma_line);
            free(new_line);
            return false;
        }
        free(comma_line);
        first_operand_token = strtok(NULL,WHITESPACES ",");
        second_operand_token = strtok(NULL,WHITESPACES ",");

        if(first_operand_token == NULL)
        {
            print_error(ERROR_HEADER, "No operand specified when 2 are needed!", current_line);
            free(new_line);
            return false;
        }
        else if (second_operand_token == NULL)
        {
            print_error(ERROR_HEADER, "1 operand specified when 2 are needed!", current_line);
            free(new_line);
            return false;
        }
        
        if(strtok(NULL,WHITESPACES ",") != NULL)
        {
            print_error(ERROR_HEADER, "Too many operands specified when 2 are needed!", current_line);
            free(new_line);
            return false;
        }

        current_instruction->instruction_index = instruction_index;
        current_instruction->first_operand = strdup(first_operand_token);
        current_instruction->first_operand_type = get_addr_method(first_operand_token);
        current_instruction->second_operand = strdup(second_operand_token);
        current_instruction->second_operand_type = get_addr_method(second_operand_token);

        if(current_instruction->first_operand_type == NO_METHOD || current_instruction->second_operand_type == NO_METHOD)
        {
            print_error(ERROR_HEADER, "Illegal label name!", current_line);
            free(new_line);
            return false;
        }
    }
    else if (current_operand_count == 1)
    {
        first_operand_token = strtok(NULL,WHITESPACES ",");
        if(first_operand_token == NULL)
        {
            print_error(ERROR_HEADER, "No operand specified when 1 is needed!", current_line);
            free(new_line);
            return false;
        }
        if(strtok(NULL,WHITESPACES ",") != NULL)
        {
            print_error(ERROR_HEADER, "Too many operands specified when 1 is needed!", current_line);
            free(new_line);
            return false;
        }        
        current_instruction->instruction_index = instruction_index;
        current_instruction->first_operand = NULL;
        current_instruction->first_operand_type = NO_METHOD;
        current_instruction->second_operand = strdup(first_operand_token);
        current_instruction->second_operand_type = get_addr_method(first_operand_token);

        if(current_instruction->second_operand_type == NO_METHOD)
        {
            print_error(ERROR_HEADER, "Illegal label name!", current_line);
            free(new_line);
            return false;
        }
    }
    else /*current_operand_count == 0*/
    {
        if(strtok(NULL,WHITESPACES ",") != NULL)
        {
            print_error(ERROR_HEADER, "Too many operands specified, no operands are needed!", current_line);
            free(new_line);
            return false;
        }
        current_instruction->instruction_index = instruction_index;
        current_instruction->first_operand = NULL;
        current_instruction->first_operand_type = NO_METHOD;
        current_instruction->second_operand = NULL;
        current_instruction->second_operand_type = NO_METHOD;
    }

    free(new_line);
    return true;
}

/*Source = 1 Target = 2*/
void insert_instruction_words(Instruction* instruction, word_list_node **instruction_list_head, symbol_table_node *symbol_table,int* IC)
{
    int temp_word_value = 0;

    /*First word*/
    if(instruction->first_operand_type != NO_METHOD)
        temp_word_value = (instruction->first_operand_type) << 4;
    if(instruction->second_operand_type != NO_METHOD)
        temp_word_value |= (instruction->second_operand_type) << 2;
    temp_word_value |= get_instruction_code(instruction->instruction_index) << 6;

    insert_word_to_end(instruction_list_head,temp_word_value,true);
    (*IC)++;

    temp_word_value = 0;
    if(instruction->first_operand_type == Immediate)
    {
        get_immediate_data(instruction->first_operand,symbol_table,&temp_word_value);
        temp_word_value <<= 2;
        insert_word_to_end(instruction_list_head,temp_word_value,true);
        (*IC)++;
    }
    else if (instruction->first_operand_type == Direct)
    {
        insert_word_to_end(instruction_list_head,0,false);
        (*IC)++;        
    }
    else if (instruction->first_operand_type == Index)
    {
        insert_word_to_end(instruction_list_head,0,false);

        /*Calculate index to temp_word_value*/
        get_array_index(instruction->first_operand,symbol_table,&temp_word_value);

        /*move 2 bits for ARE*/
        temp_word_value <<= 2;

        insert_word_to_end(instruction_list_head,temp_word_value,true);
        *IC += 2;
    }

    /*Check if registers because if both then only 1 new line*/
    if(instruction->first_operand_type == Reg && instruction->second_operand_type == Reg)
    {
        temp_word_value = get_register(instruction->first_operand) << 5;        
        temp_word_value |= get_register(instruction->second_operand) << 2;
        insert_word_to_end(instruction_list_head,temp_word_value,true);
        (*IC)++;
    }
    else if (instruction->first_operand_type == Reg)
    {
        temp_word_value = get_register(instruction->first_operand) << 5;
        insert_word_to_end(instruction_list_head,temp_word_value,true);
        (*IC)++;
    }
    else if (instruction->second_operand_type == Reg)
    {
        temp_word_value = get_register(instruction->second_operand) << 2;
        insert_word_to_end(instruction_list_head,temp_word_value,true);
        (*IC)++;
    }


    /* Now do the same check as for the first operand*/
       if(instruction->second_operand_type == Immediate)
    {
        get_immediate_data(instruction->second_operand,symbol_table,&temp_word_value);
        temp_word_value <<= 2;
        insert_word_to_end(instruction_list_head,temp_word_value,true);
        (*IC)++;
    }
    else if (instruction->second_operand_type == Direct)
    {
        insert_word_to_end(instruction_list_head,0,false);
        (*IC)++;        
    }
    else if (instruction->second_operand_type == Index)
    {
        insert_word_to_end(instruction_list_head,0,false);

        /*Calculate index to temp_word_value*/
        get_array_index(instruction->second_operand,symbol_table,&temp_word_value);

        /*move 2 bits for ARE*/
        temp_word_value <<= 2;

        insert_word_to_end(instruction_list_head,temp_word_value,true);
        *IC += 2;
    }
}

bool valid_instruction(Instruction* Instruction)
{
    Instruction_enum index = (Instruction_enum)Instruction->instruction_index;

    if(index == MOV || index == ADD || index == SUB)
    {
        return Instruction->first_operand_type != NO_METHOD &&
               Instruction->second_operand_type != NO_METHOD &&
               Instruction->second_operand_type != Immediate;
    }

    else if (index == CMP)
    {
        return Instruction->first_operand_type != NO_METHOD &&
               Instruction->second_operand_type != NO_METHOD;
    }

    else if (index == NOT || index == CLR || index == INC || index == DEC || index == RED)
    {
        return Instruction->first_operand_type == NO_METHOD &&
               Instruction->second_operand_type != NO_METHOD &&
               Instruction->second_operand_type != Immediate;
    }
    else if (index == LEA)
    {
        return Instruction->first_operand_type != NO_METHOD &&
               Instruction->first_operand_type != Immediate &&
               Instruction->first_operand_type != Reg &&
               Instruction->second_operand_type != NO_METHOD &&
               Instruction->second_operand_type != Immediate;
    }
    else if (index == JMP || index == BNE || index == JSR)
    {
        return Instruction->first_operand_type == NO_METHOD &&
               Instruction->second_operand_type != NO_METHOD &&
               Instruction->second_operand_type != Index &&
               Instruction->second_operand_type != Immediate;
    }
    else if (index == PRN)
    {
        return Instruction->first_operand_type == NO_METHOD &&
               Instruction->second_operand_type != NO_METHOD;
    }
    else if (index == RTS || index == HLT)
    {
        return Instruction->first_operand_type == NO_METHOD &&
               Instruction->second_operand_type == NO_METHOD;        
    }
    return false;

}
bool first_pass(symbol_table_node** symbol_head, char* file_name, word_list_node** instruction_list, word_list_node** data_list,int* IC, int* DC)
{
    FILE* preassembled_file;
    char* preassembled_file_name;

    /* If it's a label */
    bool is_symbol = false;

    char* line = malloc(sizeof(char) * LINE_SIZE);

	symbol_table_node* temp_symbol_node = NULL;
    Instruction *current_instruction = NULL;
    char* symbol_name;

    current_instruction = (Instruction *) malloc(sizeof(Instruction));
    preassembled_file_name = malloc(sizeof(char) * (strlen(file_name) + strlen(macro_extension) + 1));

    first_pass_error = false;
    firstpass_current_line = 0;

    if(preassembled_file_name == NULL || current_instruction == NULL)
    {
        ALLOCATION_ERROR
        exit(1);        
    }
    current_instruction->first_operand = NULL;
    current_instruction->second_operand = NULL;

    strcpy(preassembled_file_name, file_name);
    preassembled_file_name = strcat(preassembled_file_name, macro_extension);

    preassembled_file = fopen(preassembled_file_name, "r");
    if(preassembled_file == NULL)
    {
        FILE_OPEN_ERROR
        exit(1);
    }

    while (fgets(line, LINE_SIZE, preassembled_file))
    {
        ++firstpass_current_line;
        if(is_empty_line(line) || is_comment_line(line))
            continue;

       is_symbol = is_label(line,&symbol_name);
       if(is_symbol)
       {
            if(!is_legal_label_name(symbol_name, firstpass_current_line, true))
            {
                first_pass_error = true;
                continue;
            }
       }
       if(is_directive(line,define_dir,is_symbol))
       {
            if(is_symbol)
            {
                print_error(ERROR_HEADER,"Using label is not allowed in a define line!",firstpass_current_line);
                first_pass_error = true;
            }
            else
            {
                if(!extract_define(line, symbol_head))
                    first_pass_error = true; 
            }
       }
       else
       {
            if(is_directive(line,string_dir,is_symbol) || is_directive(line,data_dir,is_symbol))
            {
                if(is_symbol)
                {
                    insert_symbol_to_end(symbol_head,symbol_name,data,*DC,false,firstpass_current_line);
                }
                if(is_directive(line,string_dir,is_symbol))
                {
                    if(!extract_string(line, data_list, DC))
                        first_pass_error = true;
                }
                else /*if is data*/
                {
                    if(!extract_data(line, data_list, DC, *symbol_head))
                        first_pass_error = true;

                }
            }
            else if (is_directive(line,extern_dir,is_symbol) || is_directive(line,entry_dir,is_symbol))
            {
                if(is_symbol)
                {
                    print_error(WARNING_HEADER,"Adding a label before .extern and .entry doesn't do anything!",firstpass_current_line);
                }
                if(is_directive(line,extern_dir,is_symbol))
                {
                    if(!extract_extern(line, symbol_head))
                        first_pass_error = true;
                }
            }
            else /* If code*/
            {
                if(is_symbol)
                {
                    insert_symbol_to_end(symbol_head,symbol_name,code,*IC,false,firstpass_current_line);
                }
                
                if(parse_instruction(line, current_instruction, is_symbol,firstpass_current_line, true))
                {
                    /*Validate instruction make a function for it*/
                    if(valid_instruction(current_instruction))
                    {
                        insert_instruction_words(current_instruction,instruction_list,*symbol_head,IC);
                    }
                    else
                    {
                        print_error(ERROR_HEADER,"Source operand or destination operand type is not allowed in the instruction!",firstpass_current_line);
                        first_pass_error = true;
                    }
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
                else
                {
                    first_pass_error = true;
                }
            }
       }

    }
    temp_symbol_node = *symbol_head;
    while(temp_symbol_node != NULL)
    {
        if(temp_symbol_node->attribute == data)
        {
            temp_symbol_node->value += ((*IC) + MEMORY_START);
        }
        else if (temp_symbol_node->attribute == code)
        {
            temp_symbol_node->value += MEMORY_START;
        }
        temp_symbol_node = temp_symbol_node->next;
    }

    fclose(preassembled_file);
    free(line);
    free(preassembled_file_name);
    free(current_instruction);
    return !first_pass_error;
}
