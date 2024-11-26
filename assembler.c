// assembler.c
#include <string.h>
#include <stdlib.h>
#include "assembler.h"

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
        
        if (token && token[strlen(token) - 1] == ':') {
            token[strlen(token) - 1] = '\0';
            if (add_label(token, current_address) < 0) {
                free(input_copy);
                return ASSEMBLER_ERROR;
            }
        } else if (token) {
            // Count instructions
            current_address++;
        }
        line = strtok_r(NULL, "\n", &saveptr1);
    }
    free(input_copy);

    // Second pass - generate code
    input_copy = strdup(input);
    line = strtok_r(input_copy, "\n", &saveptr1);
    int instruction_count = 0;
    
    while (line && instruction_count < max_size) {
        char* saveptr2;
        char* token = strtok_r(line, " \t", &saveptr2);
        
        if (token && token[strlen(token) - 1] != ':') {
            if (strcmp(token, "PUSH") == 0) {
                char* value = strtok_r(NULL, " \t", &saveptr2);
                if (!value) break;
                uint16_t immediate = (uint16_t)atoi(value) & IMMEDIATE_MASK;
                output[instruction_count++] = (OP_LIT << OPCODE_SHIFT) | immediate;
            } 
            else if (strcmp(token, "ADD") == 0) {
                // For arithmetic operations, use OP_OPR as main opcode and OPR_ADD as subtype
                output[instruction_count++] = (OP_OPR << OPCODE_SHIFT) | OPR_ADD;
            }
            else if (strcmp(token, "SUB") == 0) {
                output[instruction_count++] = (OP_OPR << OPCODE_SHIFT) | OPR_SUB;
            }
            else if (strcmp(token, "MUL") == 0) {
                output[instruction_count++] = (OP_OPR << OPCODE_SHIFT) | OPR_MUL;
            }
            else if (strcmp(token, "DIV") == 0) {
                output[instruction_count++] = (OP_OPR << OPCODE_SHIFT) | OPR_DIV;
            }
            else if (strcmp(token, "MOD") == 0) {
                output[instruction_count++] = (OP_OPR << OPCODE_SHIFT) | OPR_MOD;
            }
            else if (strcmp(token, "EQL") == 0) {
                output[instruction_count++] = (OP_OPR << OPCODE_SHIFT) | OPR_EQL;
            }
            else if (strcmp(token, "NEQ") == 0) {
                output[instruction_count++] = (OP_OPR << OPCODE_SHIFT) | OPR_NEQ;
            }
            else if (strcmp(token, "LSS") == 0) {
                output[instruction_count++] = (OP_OPR << OPCODE_SHIFT) | OPR_LSS;
            }
            else if (strcmp(token, "GTE") == 0) {
                output[instruction_count++] = (OP_OPR << OPCODE_SHIFT) | OPR_GTE;
            }
            else if (strcmp(token, "GTR") == 0) {
                output[instruction_count++] = (OP_OPR << OPCODE_SHIFT) | OPR_GTR;
            }
            else if (strcmp(token, "LTE") == 0) {
                output[instruction_count++] = (OP_OPR << OPCODE_SHIFT) | OPR_LTE;
            }
            else if (strcmp(token, "LSH") == 0) {
                output[instruction_count++] = (OP_OPR << OPCODE_SHIFT) | OPR_LSH;
            }
            else if (strcmp(token, "RSH") == 0) {
                output[instruction_count++] = (OP_OPR << OPCODE_SHIFT) | OPR_RSH;
            }
            else if (strcmp(token, "IN") == 0) {
                output[instruction_count++] = (OP_IN << OPCODE_SHIFT);
            }
            else if (strcmp(token, "OUT") == 0) {
                output[instruction_count++] = (OP_OUT << OPCODE_SHIFT);
            }
            else if (strcmp(token, "CAL") == 0) {
                char* label = strtok_r(NULL, " \t", &saveptr2);
                if (!label) break;
                int target = find_label(label);
                if (target < 0) {
                    free(input_copy);
                    return ASSEMBLER_ERROR;
                }
                output[instruction_count++] = (OP_CAL << OPCODE_SHIFT) | (target & IMMEDIATE_MASK);
            }
            else if (strcmp(token, "RET") == 0) {
                output[instruction_count++] = (OP_RET << OPCODE_SHIFT);
            }
            else if (strcmp(token, "JPC") == 0) {
                char* label = strtok_r(NULL, " \t", &saveptr2);
                if (!label) break;
                int target = find_label(label);
                if (target < 0) {
                    free(input_copy);
                    return ASSEMBLER_ERROR;
                }
                output[instruction_count++] = (OP_JPC << OPCODE_SHIFT) | (target & IMMEDIATE_MASK);
            }
            else if (strcmp(token, "JMP") == 0) {
                char* label = strtok_r(NULL, " \t", &saveptr2);
                if (!label) break;
                int target = find_label(label);
                if (target < 0) {
                    free(input_copy);
                    return ASSEMBLER_ERROR;
                }
                output[instruction_count++] = (OP_JMP << OPCODE_SHIFT) | (target & IMMEDIATE_MASK);
            }
        }
        line = strtok_r(NULL, "\n", &saveptr1);
    }

    free(input_copy);
    return instruction_count;
}

int assemble_file(const char* input_file, const char* output_file) {
    // TODO: Implement file handling
    return 0;
}