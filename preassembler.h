#ifndef PREASSEMBLER_H
#define PREASSEMBLER_H

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include "macrotable.h"
#include "utils.h"
#include "globals.h"

/*Enumeration representing error types related to macros.*/
typedef enum {
    INVALID_LINE_LEN,
    TEXT_AFTER_MCR,
    TEXT_AFTER_ENDMCR,
    INVALID_MACRO_NAME,
    MACRO_NAME_ALREADY_EXISTS
} ErrorType;

#define ILLEGAL_NAME_COUNT 22

/*
  Prints an error related to the macros.
  @param type Type of the error.
  @param line_number Line number where the error occurred.
  @param macro_name Name of the macro associated with the error.
 */
void printMacroError(ErrorType type, int line_number, const char* macro_name);


/*
  Expands macros in the given file.
  @param file_name The name of the file to expand macros in.
  @return True if macros were successfully expanded, false otherwise.
*/
bool expand_macros(char* file_name);

#endif
