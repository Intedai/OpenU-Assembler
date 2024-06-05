#include "preassembler.h"

bool macro_error = false;
int current_line = 0;

void printMacroError(ErrorType type, int line_number, const char* macro_name)
{
    switch (type) {
        case TEXT_AFTER_MCR:
            fprintf(stderr, "ERROR: Line:%d: there is text after the macro start!\nCorrect macro start: mcr <macro name>\n", line_number);
            break;
        case TEXT_AFTER_ENDMCR:
            fprintf(stderr, "ERROR: Line:%d: there is text after the macro end!\nCorrect macro end: endmcr\n", line_number);
            break;
        case MACRO_NAME_ALREADY_EXISTS:
            fprintf(stderr, "ERROR: Line:%d: a macro called %s was already defined\n", line_number, macro_name);
            break;
        case INVALID_MACRO_NAME:
            fprintf(stderr, "ERROR: Line:%d: %s is an invalid macro name!\n", line_number, macro_name);
            break;
        case INVALID_LINE_LEN:
            fprintf(stderr, "ERROR: Line:%d: The line is too big!\n", line_number);
            break;

    }
}

bool is_invalid_mcr_name(char *mcr_name)
{
    const char* illegal_macro_names_list[] = {"mcr", "endmcr","mov", "cmp", "add", "sub", "not", "lea", "inc",
    "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "hlt", ".entry", ".string", ".data", ".extern", ".define"};

    int i = 0;

    if(!is_legal_label_name(mcr_name,current_line,false))
    {
        print_error(ERROR_HEADER,"Illegal macro name because it can't be a legal label name!",current_line);
    }
    /* Go over every illegal name and check if the macro name is illegal*/
    for(i=0; i < ILLEGAL_NAME_COUNT ; i++){
        if(strcmp(mcr_name, illegal_macro_names_list[i]) == 0){
            printMacroError(INVALID_MACRO_NAME,current_line,mcr_name);
            macro_error = true;
            return true;
        }
    }
    return false; 
}

bool line_too_long(const char* line)
{
    /* If last character is not a new line and size is max*/
    if(line[strlen(line) - 1] != '\n' && strlen(line) >= LINE_SIZE - 1)
    {
        printMacroError(INVALID_LINE_LEN,current_line,NULL);
        macro_error = true;
        return true;
    }
    return false;
}

char* extract_and_set_macro_name(char *input) {
    char *start, *end;
    int length;
    char *macroName;

    /* Skip whitespaces */
    for (start = input; *start && isspace(*start); ++start);

    /* Skip mcr */
    if (strncmp(start, "mcr", 3) == 0) {
        start += 3;
        
        while (*start && isspace(*start)) ++start; 
    } else {

        return NULL;
    }

    /* Find end */
    for (end = start; *end && !isspace(*end); ++end);

    /* Calculate length*/
    length = end - start;

    macroName = (char*)malloc(length + 1);
    if (macroName == NULL) {

        return NULL;
    }

    /* Copy  macro */
    strncpy(macroName, start, length);
    macroName[length] = '\0'; 

    return macroName;
}

bool is_endmcr(const char *str) {

    /* Skip whitespaces */
    while (isspace((unsigned char)*str)) {
        str++;
    }

    /* Check if line starts with endmcr */
    if (strncmp(str, "endmcr", 6) != 0) {
        return false;
    }

    str += strlen("endmcr");

    /* Skip whitespaces after endmcr */
    while (isspace((unsigned char)*str)) {
        str++;
    }

    /* Ensure no extra characters after endmcr */
    if (*str != '\0') {
        macro_error = true;
        printMacroError(TEXT_AFTER_ENDMCR,current_line,NULL);
        return false;
    }

    return true;
}

bool isMacroStart(const char *str) {

    /* Skip whitespaces*/
    while (isspace((unsigned char)*str)) {
        str++;
    }

    /* Check if line starts with mcr*/
    if (strncmp(str, "mcr", 3) != 0) {
        return false;
    }

    str += strlen("mcr");

    /* Skip whitespaces*/
    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0' || isspace((unsigned char)*str)) {
        return false;
    }

    while (*str != '\0' && !isspace((unsigned char)*str)) {
        str++;
    }

    while (isspace((unsigned char)*str)) {
        str++;
    }
    /* If there's text after mcr name make macro_error true */
    if (*str != '\0') {
        printMacroError(TEXT_AFTER_MCR,current_line,NULL);
        macro_error = true;
    }
    return *str == '\0';
}

void add_macro(FILE *source, FILE *output, macro_table_node* macro)
{
    /* Allocate memory for reading macro lines from source file*/
    char* macro_line = malloc(sizeof(char) * LINE_SIZE);
    
    int curPos;

    if(macro_line == NULL)
    {
        ALLOCATION_ERROR
        exit(1);        
    }

    /* Save current position position*/
    curPos = ftell(source);

    /* Go to beginning of macro*/
    fseek(source, macro->beginning,SEEK_SET);

    /* Put macro in output file*/
    while(ftell(source) < macro->end && fgets(macro_line, LINE_SIZE, source))
    {
        fputs(macro_line,output);
    }

    /* Go to current position*/
    fseek(source,curPos,SEEK_SET);
    free(macro_line);
}

bool expand_macros(char* file_name)
{
    /*///////*/
    FILE* source_file;
    FILE* output_file;

	macro_table_node* head = NULL;
	macro_table_node* curMacro = NULL;

    char *macroName = NULL;
	int beginning;
	int end;

    bool is_macro = false;

    char* source_file_name;
    char* output_file_name;

    char* line = malloc(sizeof(char) * LINE_SIZE);
    char* trimmed;

    macro_error = false;
    current_line = 0;

    /* Allocate memory for file names with extension */
    source_file_name = malloc(sizeof(char) * (strlen(file_name) + strlen(source_extension) + 1));
    output_file_name = malloc(sizeof(char) * (strlen(file_name) + strlen(macro_extension) + 1));

    if(source_file_name == NULL || output_file_name == NULL || line == NULL)
    {
        ALLOCATION_ERROR
        exit(1);        
    }

    /* Make file name with extension by combining */
    strcpy(source_file_name, file_name);
    strcpy(output_file_name, file_name);
    source_file_name = strcat(source_file_name, source_extension);
    output_file_name = strcat(output_file_name, macro_extension);

    source_file = fopen(source_file_name, "r");
    output_file = fopen(output_file_name, "w");


    if(source_file == NULL || output_file == NULL)
    {
        FILE_OPEN_ERROR
        exit(1);
    }

    /* Iterate through file lines */
    while (fgets(line, LINE_SIZE, source_file) && !macro_error)
    {
        ++current_line;

        if(line_too_long(line))
        {
            break;
        }
        
        trimmed = trimString(line);

        /* Inside macro definition */
        if(is_macro)
        {
            /* If reached end */
            if(is_endmcr(line))
            {
                /* Not in macro anymore turn off flag */
                is_macro = false;

                /* Get end cursor without endmcr to put in macrotable*/
                end = ftell(source_file) - strlen(line);
                
                insert_to_end(&head, macroName, beginning, end);
            }
        }
        else
        {
            /* If start of macro definition */
            if(isMacroStart(line))
            {
                is_macro = true;

                /* Extract macro */
                macroName = extract_and_set_macro_name(line);
                /* Check if macro is legal*/
                if(is_invalid_mcr_name(macroName))
                {
                    break;
                }
                /* If macro already exists: */
                else if (search_node(head,macroName) != NULL)
                {
                    printMacroError(MACRO_NAME_ALREADY_EXISTS,current_line,macroName);
                    macro_error = true;
                    break;
                }
                /* Get beginning of macro definition */
                beginning = ftell(source_file);
            }
            /**Add to search node to extract only name no spaces*/
            else if((curMacro = search_node(head, trimmed)) != NULL)/*if macro name, expand*/
            {
                /*adds macro definition instead of name*/
                add_macro(source_file, output_file, curMacro);
            }
            else
            {
                /* Nothing happened just put the normal line from source to output*/
                fputs(line, output_file);
            }
        }
        /* Free trimmed that is allocated every loop */
        free(trimmed);
    }

    free(line);
    free(source_file_name);
    fclose(source_file);
    fclose(output_file);
    free_macro_table(head);
    if(macro_error){
        remove(output_file_name);
        free(output_file_name);
        return false;
    }
    free(output_file_name);
    
    return true;
}
