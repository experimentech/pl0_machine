// assembler.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assembler.h"

#define MAX_INSTRUCTIONS 1024  // Add this at the top of assembler.c or in assembler.h

static LabelTable label_table;

static void init_label_table() {
    label_table.count = 0;
}

static int add_label(const char* name, int address) {
    if (label_table.count >= MAX_LABELS) return -1;
    strncpy(label_table.labels[label_table.count].name, name, MAX_LABEL_LEN-1);
    label_table.labels[label_table.count].address = address;
    label_table.count++;
    return 0;
}

static int find_label(const char* name) {
    for (int i = 0; i < label_table.count; i++) {
        if (strcmp(label_table.labels[i].name, name) == 0) {
            return label_table.labels[i].address;
        }
    }
    return -1;
}

static const Instruction instruction_table[] = {
    // Stack/Memory
    {"PUSH", OP_LIT, 0,       INSTR_LIT},
    {"LOD",  OP_LOD, 0,       INSTR_LIT},
    {"STO",  OP_STO, 0,       INSTR_LIT},
    
    // Arithmetic 
    {"ADD",  OP_OPR, OPR_ADD, INSTR_ARITH},
    {"SUB",  OP_OPR, OPR_SUB, INSTR_ARITH},
    {"MUL",  OP_OPR, OPR_MUL, INSTR_ARITH},
    {"DIV",  OP_OPR, OPR_DIV, INSTR_ARITH},
    {"MOD",  OP_OPR, OPR_MOD, INSTR_ARITH},
    
    // Comparisons
    {"EQL",  OP_OPR, OPR_EQL, INSTR_ARITH},
    {"NEQ",  OP_OPR, OPR_NEQ, INSTR_ARITH},
    {"LSS",  OP_OPR, OPR_LSS, INSTR_ARITH},
    {"GTE",  OP_OPR, OPR_GTE, INSTR_ARITH},
    {"GTR",  OP_OPR, OPR_GTR, INSTR_ARITH},
    {"LTE",  OP_OPR, OPR_LTE, INSTR_ARITH},
    
    // Bit ops
    {"LSH",  OP_OPR, OPR_LSH, INSTR_ARITH},
    {"RSH",  OP_OPR, OPR_RSH, INSTR_ARITH},
    
    // Control flow
    {"JMP",  OP_JMP, 0,       INSTR_JUMP},
    {"JPC",  OP_JPC, 0,       INSTR_JUMP},
    {"CAL",  OP_CAL, 0,       INSTR_JUMP},
    {"RET",  OP_RET, 0,       INSTR_BASIC},
    
    // I/O
    {"IN",   OP_IN,  0,       INSTR_BASIC},
    {"OUT",  OP_OUT, 0,       INSTR_BASIC},
    
    {NULL,   0,      0,       0}  // Sentinel
};

static const Instruction* find_instruction(const char* mnemonic) {
    for (const Instruction* i = instruction_table; i->mnemonic; i++) {
        if (strcmp(i->mnemonic, mnemonic) == 0) {
            return i;
        }
    }
    return NULL;
}

AssemblerResult assemble_string(const char* input, uint16_t* output, int max_size) {
    if (!input || !output || max_size < 1) {
        return ASSEMBLER_INVALID_PARAMS;
    }

    init_label_table();

    // First pass - collect labels and count instructions
    char* input_copy = strdup(input);
    char* saveptr1;
    char* line = strtok_r(input_copy, "\n", &saveptr1);
    int current_address = 0;
    
    while (line) {
        char* saveptr2;
        char* token = strtok_r(line, " \t", &saveptr2);
        
        if (!token) {
            line = strtok_r(NULL, "\n", &saveptr1);
            continue;
        }

        // Skip comments and empty lines
        if (token[0] == ';' || token[0] == '#') {
            line = strtok_r(NULL, "\n", &saveptr1);
            continue;
        }

        if (token[strlen(token) - 1] == ':') {
            // Handle label definition - don't increment address
            token[strlen(token) - 1] = '\0';
            if (add_label(token, current_address) < 0) {
                free(input_copy);
                return ASSEMBLER_ERROR;
            }
        } else {
            // Look up instruction in table
            const Instruction* instr = find_instruction(token);
            if (instr) {
                current_address++;  // Count instruction
                if (instr->type == INSTR_LIT || instr->type == INSTR_JUMP) {
                    // Skip argument token
                    char* arg = strtok_r(NULL, " \t", &saveptr2);
                    if (!arg) {
                        free(input_copy);
                        return ASSEMBLER_ERROR;  // Missing required argument
                    }
                }
            }
        }

        line = strtok_r(NULL, "\n", &saveptr1);
    }

    fprintf(stderr, "First pass complete: found %d labels, counted %d instructions\n", 
            label_table.count, current_address);

    // Second pass - generate code
    free(input_copy);
    input_copy = strdup(input);
    line = strtok_r(input_copy, "\n", &saveptr1);
    int instruction_count = 0;

    while (line && instruction_count < max_size) {
        char* saveptr2;
        char* token = strtok_r(line, " \t", &saveptr2);
        
        if (!token || token[strlen(token) - 1] == ':') {
            line = strtok_r(NULL, "\n", &saveptr1);
            continue;
        }

        const Instruction* instr = find_instruction(token);
        if (!instr) {
            free(input_copy);
            return ASSEMBLER_ERROR;
        }

        switch (instr->type) {
            case INSTR_LIT:
                char* value = strtok_r(NULL, " \t", &saveptr2);
                if (!value) break;
                output[instruction_count++] = (instr->opcode << OPCODE_SHIFT) | 
                                            (atoi(value) & IMMEDIATE_MASK);
                break;
                
            case INSTR_JUMP:
                char* label = strtok_r(NULL, " \t", &saveptr2);
                if (!label) break;
                int target = find_label(label);
                if (target < 0) {
                    free(input_copy);
                    return ASSEMBLER_ERROR;
                }
                output[instruction_count++] = (instr->opcode << OPCODE_SHIFT) | 
                                            (target & IMMEDIATE_MASK);
                break;
                
            case INSTR_ARITH:
                output[instruction_count++] = (instr->opcode << OPCODE_SHIFT) | 
                                            instr->subtype;
                break;
                
            case INSTR_BASIC:
                output[instruction_count++] = (instr->opcode << OPCODE_SHIFT);
                break;
        }
        
        line = strtok_r(NULL, "\n", &saveptr1);
    }

    free(input_copy);
    return instruction_count;
}

AssemblerResult assemble_file(const char* input_file, const char* output_file) {
    // Open input file
    FILE* in = fopen(input_file, "r");
    if (!in) {
        fprintf(stderr, "Error: Could not open input file %s\n", input_file);
        return ASSEMBLER_INVALID_PARAMS;
    }

    // Get file size
    fseek(in, 0, SEEK_END);
    long file_size = ftell(in);
    fseek(in, 0, SEEK_SET);

    // Read entire file into buffer
    char* input_buffer = (char*)malloc(file_size + 1);
    if (!input_buffer) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(in);
        return ASSEMBLER_ERROR;
    }
    
    size_t bytes_read = fread(input_buffer, 1, file_size, in);
    input_buffer[bytes_read] = '\0';
    fclose(in);

    // Allocate output buffer for machine code
    uint16_t output_buffer[MAX_INSTRUCTIONS];
    
    // Assemble the code - match AssemblerResult type
    AssemblerResult result = assemble_string(input_buffer, output_buffer, MAX_INSTRUCTIONS);
    if (result <= 0) {
        fprintf(stderr, "Error: Assembly failed\n");
        free(input_buffer);
        return result;
    }

    // Write output
    FILE* out = fopen(output_file, "wb");
    if (!out) {
        fprintf(stderr, "Error: Could not open output file %s\n", output_file);
        free(input_buffer);
        return ASSEMBLER_FILE_ERROR;
    }

    // Write machine code - use result as count
    size_t words_written = fwrite(output_buffer, sizeof(uint16_t), result, out);
    if (words_written != result) {
        fprintf(stderr, "Error: Failed to write all instructions\n");
        free(input_buffer);
        fclose(out);
        return ASSEMBLER_FILE_ERROR;
    }

    free(input_buffer);
    fclose(out);
    return ASSEMBLER_SUCCESS;
}