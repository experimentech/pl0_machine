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
    {"LIT",  OP_LIT, 0,       INSTR_LIT},
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
    {"RJMP", OP_RJMP,0,       INSTR_JUMP},
    {"RJPC", OP_RJPC,0,       INSTR_JUMP},
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

static uint16_t encode_instruction(const Instruction* instr, const char* arg) {
    if (!instr) return 0;

    switch (instr->type) {
        case INSTR_LIT:
            return (instr->opcode << OPCODE_SHIFT) | (atoi(arg) & IMMEDIATE_MASK);
            
        case INSTR_ARITH:
            return (instr->opcode << OPCODE_SHIFT) | (instr->subtype & IMMEDIATE_MASK);
            
        case INSTR_BASIC:
            return (instr->opcode << OPCODE_SHIFT);
            
        default:
            fprintf(stderr, "Error: Unknown instruction type %d\n", instr->type);
            return 0;
    }
}

AssemblerResult assemble_string(const char* input, uint16_t* output, int max_size) {
    fprintf(stderr, "Debug: Starting assembly of:\n%s\n", input);
    
    // First pass
    fprintf(stderr, "Debug: First pass - collecting labels\n");
    init_label_table();
    int current_address = 0;
    int skip_mode = 0;
    
    char* input_copy = strdup(input);
    char* saveptr1 = NULL;
    char* saveptr2 = NULL;
    char* line = strtok_r(input_copy, "\n", &saveptr1);

    // First pass - collect labels and count reachable instructions
    while (line) {
        char* token = strtok_r(line, " \t", &saveptr2);
        if (token) {
            if (token[strlen(token) - 1] == ':') {
                // Label points to current instruction
                token[strlen(token) - 1] = '\0';
                fprintf(stderr, "Debug: Found label '%s' at address %d\n", 
                        token, current_address);
                add_label(token, current_address);
                skip_mode = 0;  // Reset skip mode at label
            } else {
                const Instruction* instr = find_instruction(token);
                if (instr) {
                    if (!skip_mode) {
                        fprintf(stderr, "Debug: Found instruction '%s' at address %d\n", 
                                token, current_address);
                        current_address++;  // Only increment for reachable instructions
                        
                        // Only enable skip mode after unconditional jumps
                        if (instr->type == INSTR_JUMP && instr->opcode == OP_JMP) {
                            fprintf(stderr, "Debug: Enabling skip mode after JMP\n");
                            skip_mode = 1;
                        }
                    } else {
                        fprintf(stderr, "Debug: Skipping unreachable instruction '%s'\n", token);
                    }
                }
            }
        }
        line = strtok_r(NULL, "\n", &saveptr1);
    }

    // Second pass - generate code only for reachable instructions
    skip_mode = 0;
    fprintf(stderr, "Debug: Second pass - generating code\n");
    free(input_copy);
    input_copy = strdup(input);
    saveptr1 = NULL;
    line = strtok_r(input_copy, "\n", &saveptr1);
    int instruction_count = 0;

    while (line && instruction_count < max_size) {
        saveptr2 = NULL;
        char* token = strtok_r(line, " \t", &saveptr2);
        if (token) {
            if (token[strlen(token) - 1] == ':') {
                token[strlen(token) - 1] = '\0';
                fprintf(stderr, "Debug: Found label '%s', resetting skip mode\n", token);
                skip_mode = 0;
            } else if (!skip_mode) {
                const Instruction* instr = find_instruction(token);
                if (instr) {
                    char* arg = strtok_r(NULL, " \t", &saveptr2);
                    fprintf(stderr, "Debug: Generating code for '%s'%s%s at position %d\n",
                            token, arg ? " " : "", arg ? arg : "", instruction_count);
                    
                    if (instr->type == INSTR_JUMP) {
                        int target = find_label(arg);
                        output[instruction_count] = (instr->opcode << 12) | (target & 0xFFF);
                        fprintf(stderr, "Debug: Jump instruction to label '%s' (address %d)\n",
                                arg, target);
                        if (instr->opcode == OP_JMP) {
                            fprintf(stderr, "Debug: Enabling skip mode after JMP\n");
                            skip_mode = 1;
                        }
                    } else {
                        output[instruction_count] = encode_instruction(instr, arg);
                    }
                    instruction_count++;
                }
            } else {
                fprintf(stderr, "Debug: Skipping instruction '%s' due to skip_mode\n", token);
            }
        }
        line = strtok_r(NULL, "\n", &saveptr1);
    }

    fprintf(stderr, "Debug: Generated %d instructions\n", instruction_count);
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
    fprintf(stderr, "Debug: Opened input file %s\n", input_file);

    // Get file size
    fseek(in, 0, SEEK_END);
    long file_size = ftell(in);
    fseek(in, 0, SEEK_SET);
    fprintf(stderr, "Debug: File size: %ld bytes\n", file_size);

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
    fprintf(stderr, "Debug: Read %zu bytes\n", bytes_read);
    fprintf(stderr, "Debug: Input buffer contents:\n%s\n", input_buffer);

    // Allocate output buffer for machine code
    uint16_t output_buffer[MAX_INSTRUCTIONS];
    fprintf(stderr, "Debug: Allocated output buffer for %d instructions\n", MAX_INSTRUCTIONS);
    
    // Assemble the code
    fprintf(stderr, "Debug: Starting assembly...\n");
    AssemblerResult result = assemble_string(input_buffer, output_buffer, MAX_INSTRUCTIONS);
    fprintf(stderr, "Debug: Assembly returned %d\n", result);
    
    if (result <= 0) {
        fprintf(stderr, "Error: Assembly failed with code %d\n", result);
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
    fprintf(stderr, "Debug: Opened output file %s\n", output_file);

    // Write machine code
    size_t words_written = fwrite(output_buffer, sizeof(uint16_t), result, out);
    fprintf(stderr, "Debug: Writing %d words...\n", result);
    
    // Fix format specifier for words_written comparison
    if (words_written != (size_t)result) {
        fprintf(stderr, "Error: Failed to write all instructions (wrote %zu of %d)\n", 
                words_written, result);
        free(input_buffer);
        fclose(out);
        return ASSEMBLER_FILE_ERROR;
    }
    fprintf(stderr, "Debug: Successfully wrote %zu words\n", words_written);

    // Debug dump of generated code
    fprintf(stderr, "Debug: Generated instructions:\n");
    for (size_t i = 0; i < words_written; i++) {
        fprintf(stderr, "  %04d: 0x%04X\n", (int)i, output_buffer[i]);
    }

    free(input_buffer);
    fclose(out);
    return result;  // Return number of instructions assembled
}