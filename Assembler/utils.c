#include "utils.h"

char* strdup(const char* string){
    /* Get size of string with \0 */
    int len = strlen(string)+1;

    /* Allocate memory*/
    char* duplicate=malloc(sizeof(char) * len);

    if(duplicate == NULL){
        ALLOCATION_ERROR
        exit(1);
    }

    /* Copy to memory*/
    memcpy(duplicate, string, len);
    return duplicate;
}

char* trimString(const char* original) {
    int start = 0; 
    int end; 
    int length; 
    char* trimmed; 

    /* Get start index */
    while (isspace((unsigned char) original[start])) start++;

    /* Get end index */
    end = strlen(original) - 1;
    while (end >= 0 && isspace(original[end])) end--;

    /* Calculate length*/
    length = end - start + 1;
    if (length < 0) length = 0;

    /* Allocate memory from trimmed string*/
    trimmed = (char*)malloc(length + 1);
    if(trimmed == NULL)
    {
        ALLOCATION_ERROR
        exit(1);
    }

    /*copy from the memory with the start index and length*/
    memcpy(trimmed, &original[start], length);

    /* NULL terminate the string*/
    trimmed[length] = '\0';

    return trimmed;
}

bool file_exists(const char *file_name) {
    char *file_with_extension;
    FILE *file;

    /* File name with .as extension*/
    file_with_extension = malloc(sizeof(char) * (strlen(file_name) + strlen(source_extension) + 1));

    if(file_with_extension == NULL)
        ALLOCATION_ERROR
    
    /* set file to file name with .as*/
    strcpy(file_with_extension, file_name);
    file_with_extension = strcat(file_with_extension, source_extension);

    /* If file opens it exists*/
    file = fopen(file_with_extension, "r");
    if (file != NULL) {
        fclose(file);
        return true;
    }
    /* File didnt open, probably doesn't exist!*/
    fprintf(stderr, "Error opening %s, File %s probably does not exist!\n", file_with_extension,file_with_extension);
    free(file_with_extension);
    return false;
}

bool is_directive(char *line, char *directive, bool contains_label)
{
    bool has_directive = false;
    
    char *new_line = trimString(line);
    char *temp = new_line;

    /* Skip label*/
    if(contains_label)
    {
        while(*temp != ':')
        {
            ++temp;
        }
        ++temp;
    }

    while (isspace(*temp))
		++temp;
    
    /* check if the memory has directive name and then whitespace meaning it is the directive*/
	if (memcmp(temp,directive,strlen(directive)) == 0)
    {
        temp += strlen(directive);
        if(isspace(*temp))
            has_directive = true;
    }
    free(new_line);
    return has_directive;
}

bool is_label(char* line, char** label_name)
{
    /* Allocate duplicated line trimmed from both sides */
    char *new_line = trimString(line);
    char *temp;
    bool contains_label = false;

    temp = new_line;

    /* Iterate through the new line, if theres : its a label*/
    while(*temp != '\0')
    {
        if(*temp == ':')
        {
            /* change flag to contains label */
            contains_label = true;
            *temp = '\0';
        }
        ++temp;
    }
    if(!contains_label)
    {
        free(new_line);
        return false;
    }

    /* If label name isnt null allocate memory and copy*/
    if(label_name != NULL)
    {
        *label_name = malloc(sizeof(char) * (strlen(new_line) + 1));
        if (*label_name == NULL) 
        {
            ALLOCATION_ERROR
            exit(1);
        }
        strcpy(*label_name, new_line);
    }
    free(new_line);
    return true;
}

const char* get_method_name(AddrMethodsOptions method) {
    /** Get the name of the address method */
    switch(method) {
        case Immediate: return "Immediate";
        case Direct: return "Direct";
        case Index: return "Index";
        case Reg: return "Reg";
        case NO_METHOD: return "NO_METHOD";
        default: return "Unknown";
    }
}


void print_instruction_list(Instruction* head) {
    Instruction* temp = head;
    /* Iterate through instruction list and print every struct*/
    if (temp != NULL) {
        printf("CURRENT INSTRUCTION: Index: %d, Operand 1: %s, Type 1: %s, Operand 2: %s, Type 2: %s\n",
            temp->instruction_index,
            temp->first_operand, get_method_name(temp->first_operand_type),
            temp->second_operand, get_method_name(temp->second_operand_type));
    }
}

int count_char_in_string(char *string, char character)
{
    int i;
    int count = 0;
    
    /* Bad input */
    if(string == NULL)
        return count;

    /* Iterate through the string and count occurences of character*/
    for(i = 0; i < strlen(string); ++i)
    {
        if(string[i] == character)
            ++count;
    }

    return count;
}
bool is_legal_label_name(char *operand, int current_line, bool print_errors)
{
    int i;

    /* Bad input */
    if(operand == NULL)
    {
        if(print_errors)
            print_error(ERROR_HEADER,"ERROR BAD INPUT!",current_line);
        return false;
    }
    /* Bigger than the max label size specified in the sheet */
    else if(strlen(operand) > LABEL_SIZE)
    {
        if(print_errors)
            print_error(ERROR_HEADER, "Symbol name is too big, maximum length is 31!",current_line);
        return false;        
    }
    /* Without any characters it's illegal */
    else if (strlen(operand) == 0)
    {
        if(print_errors)
            print_error(ERROR_HEADER, "Symbol name cannot be empty!",current_line);
        return false;
    }

    /* Check if first char is a letter */
    else if (!isalpha(*operand))
    {
        if(print_errors)
            print_error(ERROR_HEADER, "Symbol must start with a letter!",current_line);
        return false;
    }
    /* Check if the string consists only of alphanumerical characters */
    for(i = 0; i < strlen(operand); ++i)
    {
        if(!isalnum(operand[i]))
        {
            if(print_errors)
                print_error(ERROR_HEADER, "Symbol must consist of alphanumeric letters!",current_line);
            return false;
        }
    }
    /* Passed all the checks the name is legal */
    return true;
    
}

bool is_empty_line(char *line)
{
    /* Iterate through the line if only whitespaces its empty*/
    int i;
    for(i = 0; i < strlen(line); ++i)
    {
        if(!isspace(line[i]))
            return false;
    }
    return true;
}

bool is_comment_line(char *line)
{   /*skip whitespaces by trimming the string and check if starts with ;*/
    bool return_bool;
    char *trimmed_line = trimString(line);
    if(*trimmed_line == ';')
    {
        return_bool = true;
    }
    else
    {
        return_bool = false;
    }
    free(trimmed_line);
    return return_bool;
}

void print_error(char* header, char* message, int current_line)
{
    fprintf(stderr, "%s: Line:%d: %s\n",header,current_line,message);
}

bool verify_commas(char *line, int current_line) {
    char *new_line = trimString(line);
    char *temp = new_line;
    bool after_comma;

    /* Check if string starts or ends with comma*/
    if (*temp == ',') {
        print_error(ERROR_HEADER, "cannot start with a comma", current_line);
        free(new_line);
        return false;
    }
    else if(temp[strlen(temp) - 1] == ',')
    {
        print_error(ERROR_HEADER, "cannot end with a comma", current_line);
        free(new_line);
        return false;
    }
    
    /* Currently not on a comma */
    after_comma = false;

    while (*temp) {
        if (*temp == ',') {
            if (after_comma) {
                print_error(ERROR_HEADER, "cannot have consecutive commas", current_line);
                return false;
            }
            /* Next is new number */
            after_comma = true;
        } else if (!isspace(*temp)) {
            /* Check if it's the first character or after a comma */
            if (after_comma || temp == new_line) { 
                /* Find end of number */
                while (*temp && *temp != ',') {
                    if (isspace(*temp)) {
                        /* Spaces dont matter so we skip them*/
                        while (isspace(*temp)) temp++;
                        /* Invalid if not ending with comma or end of string */
                        if (*temp != ',' && *temp != '\0') 
                        {
                            print_error(ERROR_HEADER, "space between two items without a comma", current_line);
                            return false; 
                        }
                    } else {
                        temp++;
                    }
                }

                /* Check if ended on a comma or not*/
                after_comma = (*temp == ',');
                if (!after_comma) {
                    /* This is end of string */
                    break;
                }
            } else {
                free(new_line);
                print_error(ERROR_HEADER, "Illegal placement of characters between space", current_line);
                return false;
            }
        }
        temp++;
    }

    free(new_line);
    /* Return true if its not on a comma currently meaning it is correct*/
    return !after_comma; 
}
