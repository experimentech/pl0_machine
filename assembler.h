// assembler.h
#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdint.h>
#include "assembler_types.h"

// Main opcodes (4 bits)
#define OP_LIT 0x1   // Load literal (changed from 0x0)
#define OP_OPR 0x2   // Arithmetic operations (changed from 0x1)

// Opcodes matching pl0_machine.v
#define OP_LOD 0x2   // Load from memory
#define OP_STO 0x3   // Store to memory
#define OP_CAL 0x4   // Call subroutine
#define OP_RET 0x5   // Return from subroutine
#define OP_JMP 0x6   // Unconditional jump
#define OP_JPC 0x7   // Jump if zero
#define OP_IN  0x8   // Input
#define OP_OUT 0x9   // Output

// OPR subtypes match pl0_machine.v implementation
#define OPR_RET 0x00  // Return - matches 8'h00
#define OPR_ADD 0x01  // ADD    - matches 8'h01
#define OPR_SUB 0x02  // SUB    - matches 8'h02
#define OPR_MUL 0x03  // MUL    - matches 8'h03
#define OPR_DIV 0x04  // DIV  - Division
#define OPR_MOD 0x05  // MOD  - Modulo
#define OPR_EQL 0x06  // EQL  - Equal
#define OPR_NEQ 0x07  // NEQ  - Not equal
#define OPR_LSS 0x08  // LSS  - Less than
#define OPR_GTE 0x09  // GTE  - Greater or equal
#define OPR_GTR 0x0A  // GTR  - Greater than
#define OPR_LTE 0x0B  // LTE  - Less or equal
#define OPR_LSH 0x0C  // LSH  - Left shift
#define OPR_RSH 0x0D  // RSH  - Right shift
// Instruction format constants
#define OPCODE_SHIFT 12
#define IMMEDIATE_MASK 0x0FFF  // Lower 12 bits for immediate value

#define MAX_LABELS 100
#define MAX_LABEL_LEN 32

typedef struct {
    char name[MAX_LABEL_LEN];
    int address;
} Label;

typedef struct {
    Label labels[MAX_LABELS];
    int count;
} LabelTable;

typedef enum {
    INSTR_ARITH,  // Arithmetic (uses OPR)
    INSTR_JUMP,   // Jump (needs label)
    INSTR_LIT,    // Literal (needs value)
    INSTR_BASIC   // Basic instruction (no args)
} InstrType;

typedef struct {
    const char* mnemonic;
    uint8_t opcode;
    uint8_t subtype;
    InstrType type;
} Instruction;

// Function declarations
AssemblerResult assemble_string(const char* input, uint16_t* output, int max_size);
AssemblerResult assemble_file(const char* input_file, const char* output_file);

#endif