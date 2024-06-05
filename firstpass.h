#ifndef FIRSTPASS_H
#define FIRSTPASS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "globals.h"
#include "symboltable.h"
#include "wordlist.h"

/*
  Performs the first pass of the assembly process.
  @param symbol_head Pointer to the head of the symbol table.
  @param file_name The name of the file being processed.
  @param instruction_list Pointer to the head of the instruction list.
  @param data_list Pointer to the head of the data list.
  @param IC Pointer to the instruction counter.
  @param DC Pointer to the data counter.
  @return True if the first pass is successful, otherwise false.
 */
bool first_pass(symbol_table_node** symbol_head, char* file_name, word_list_node** instruction_list, word_list_node** data_list,int* IC, int* DC);

/*
  Parses an assembly instruction from a line of code.
  @param line The line of code to parse.
  @param current_instruction Pointer to the Instruction struct to store the parsed instruction.
  @param contains_label Indicates if the line contains a label.
  @param current_line The current line number being processed.
  @return True if the instruction parsing is successful, otherwise false.
 */
bool parse_instruction(char* line, Instruction *current_instruction, bool contains_label, int current_line, bool print_errors);
#endif
