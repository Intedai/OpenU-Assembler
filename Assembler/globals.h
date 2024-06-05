#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>


#define LINE_SIZE 82
#define LABEL_SIZE 31
#define MEMORY_START 100
#define MAX_MEMORY 4096

#define MAX_12_BITS 2047
#define MIN_12_BITS -2048

#define DECIMAL 10

#define ARE_R 2
#define ARE_E 1

#define WORD 1
#define TWO_WORDS 2

#define ERROR_HEADER "ERROR"
#define WARNING_HEADER "WARNING"

#define ALLOCATION_ERROR fprintf(stderr, "YOUR COMPUTER ERROR: Failed to allocate memory!\n");
#define FILE_OPEN_ERROR fprintf(stderr, "YOUR COMPUTER ERROR: Couldn't open file!\n");
#define FILE_REMOVE_ERROR fprintf(stderr, "YOUR COMPUTER ERROR: Couldn't remove file!\n\n");

#define source_extension ".as"
#define macro_extension ".am"
#define externals_extension ".ext"
#define entries_extension ".ent"
#define object_extension ".ob"

#define define_dir ".define"
#define string_dir ".string"
#define data_dir ".data"
#define extern_dir ".extern"
#define entry_dir ".entry"

#define WHITESPACES " \t\n"

typedef enum {
    MOV,
    CMP,
    ADD,
    SUB,
    LEA,
    NOT,
    CLR,
    INC,
    DEC,
    JMP,
    BNE,
    RED,
    PRN,
    JSR,
    RTS,
    HLT,
    UNDEFINED = -1
} Instruction_enum;

typedef enum Register{
    r0 = 0,
    r1,
    r2,
    r3,
    r4,
    r5,
    r6,
    r7,
    NOT_REG = -1
} Register;

typedef enum {
    Immediate,
    Direct,
    Index,
    Reg,
    NO_METHOD = -1
} AddrMethodsOptions;

typedef struct Instruction
{
    int instruction_index;
    char* first_operand;
    AddrMethodsOptions first_operand_type;
    char* second_operand;
    AddrMethodsOptions second_operand_type;
} Instruction;

#define INSTRUCTIONS {"mov", "cmp", "add", "sub", "lea", "not", "clr","inc", "dec", "jmp", "bne", "red", "prn", "jsr","rts", "hlt"}
#define INSTRUCTION_COUNT 16

typedef enum bool {
    false,
    true
} bool;

#endif
