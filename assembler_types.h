// assembler_types.h
#ifndef ASSEMBLER_TYPES_H
#define ASSEMBLER_TYPES_H

typedef enum {
    ASSEMBLER_ERROR = 0,
    ASSEMBLER_SUCCESS = 1,
    ASSEMBLER_INVALID_PARAMS = -1,
    ASSEMBLER_FILE_ERROR = -2,
    ASSEMBLER_SYMBOL_ERROR = -3  // For label/symbol table errors
} AssemblerResult;

#endif