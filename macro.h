// macro.h
#ifndef MACRO_H
#define MACRO_H

#include <stdint.h>
#include "assembler_types.h"

#define MAX_MACRO_NAME 32
#define MAX_MACRO_BODY 1024
#define MAX_PARAMS 8
#define MAX_MACROS 100

typedef struct {
    char name[MAX_MACRO_NAME];
    char* body;
    int param_count;
    char params[MAX_PARAMS][MAX_MACRO_NAME];
} Macro;

typedef struct {
    Macro macros[MAX_MACROS];
    int count;
} MacroTable;

// Function declarations
AssemblerResult process_macro(const char* line, MacroTable* table);
char* expand_macro(const char* name, char** args, int arg_count, MacroTable* table);
AssemblerResult include_file(const char* filename, MacroTable* table);

#endif