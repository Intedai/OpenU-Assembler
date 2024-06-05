#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include <stdio.h>

/*
  This function duplicate a string
  @param src The source string to duplicate.
  @return A pointer to the duplicated string.
*/
char* strdup(const char* src);

/*
  This function trims leading and trailing whitespace characters from a string.
  @param original The original string to trim.
  @return A pointer to the trimmed string.
*/
char* trimString(const char* original);

/*
  This function checks if a line contains a directive.
  @param line The line to check for the directive.
  @param directive The directive to search for in the line.
  @param contains_label Flag indicating if the line contains a label.
  @return True if the directive is found in the line, false otherwise.
*/
bool is_directive(char *line, char *directive, bool contains_label);

/*
  This function checks if a file exists.
  @param file_name The name of the file to check for existence.
  @return True if the file exists, false otherwise.
*/
bool file_exists(const char *file_name);

/*
  This function checks if a line contains a label and extracts it.
  @param line The line to check for the label.
  @param label_name that will be set to the label if found.
  @return True if a label is found, false otherwise.
*/
bool is_label(char* line, char** label_name);

/*
  This function prints a linked list of instructions.
  @param head Pointer to the head of the instruction linked list.
*/
void print_instruction_list(Instruction* head);

/*
  This function checks if a label name is legal according to the assembler sheet
  @param operand The operand containing the label name.
  @param current_line The current line number in the source file.
  @param print_errors Flag indicating whether to print errors.
  @return True if the label name is legal, false otherwise.
*/
bool is_legal_label_name(char *operand, int current_line, bool print_errors);

/*
  This function checks if a line is empty.
  @param line The line to check.
  @return True if the line is empty, false otherwise.
 */
bool is_empty_line(char *line);

/*
  This function checks if a line is a comment.
  @param line The line to check.
  @return True if the line is a comment, false otherwise.
*/
bool is_comment_line(char *line);

/*
  This function counts the amount of a character in a string.
  @param string The string to search for occurrences of the character.
  @param character The character to count occurrences of.
  @return The number of occurrences of the character in the string.
*/
int count_char_in_string(char *string, char character);

/*
  This function prints an error message.
  @param header The header of the error message.
  @param message The error message.
  @param current_line The line number where the error occurred.
*/
void print_error(char* header, char* message, int current_line);

/*
  This function verifies if commas are properly placed in a line.
  @param line The line to verify commas in.
  @param current_line The current line number in the source file.
  @return True if commas are properly placed, false otherwise.
*/
bool verify_commas(char *line, int current_line);

#endif
