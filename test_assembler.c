// test_assembler.c
#include <stdio.h>
#include <assert.h>
#include "assembler.h"

void test_basic_stack_operation() {
    const char* input = "PUSH 42\n";
    uint16_t output[1];
    int result = assemble_string(input, output, 1);
    assert(result == 1);
    assert(output[0] == ((OP_LIT << 12) | 42));
}

void test_add_operation() {
    const char* input = "PUSH 5\nPUSH 3\nADD\n";
    uint16_t output[3];
    int result = assemble_string(input, output, 3);
    assert(result == 3);
    assert(output[0] == ((OP_LIT << 12) | 5));
    assert(output[1] == ((OP_LIT << 12) | 3));
    assert(output[2] == ((OP_OPR << 12) | OPR_ADD));  // Changed this line
}

void test_instruction_encoding() {
    // Test LIT instruction
    assert((OP_LIT << 12 | 42) == 0x102A);
    
    // Test ADD operation - uses OP_OPR with OPR_ADD subtype
    assert((OP_OPR << 12 | OPR_ADD) == 0x2001);
}

void test_jump_operations() {
    // Test unconditional jump with label
    const char* input = 
        "start:\n"
        "PUSH 1\n"
        "JMP end\n"
        "PUSH 2\n"  // Should be skipped
        "end:\n"
        "PUSH 3\n";
        
    uint16_t output[4];
    int result = assemble_string(input, output, 4);
    
    assert(result == 3);
    assert(output[0] == ((OP_LIT << 12) | 1));  // PUSH 1
    assert(output[1] == ((OP_JMP << 12) | 2));  // JMP to instruction 2 (PUSH 3)
    assert(output[2] == ((OP_LIT << 12) | 3));  // PUSH 3
}

void test_conditional_jump() {
    const char* input =
        "PUSH 0\n"       // Push comparison value
        "JPC skip\n"     // Skip next if zero
        "PUSH 1\n"       // Should be skipped
        "skip:\n"
        "PUSH 2\n";

    uint16_t output[4];
    int result = assemble_string(input, output, 4);
    
    assert(result == 3);
    assert(output[0] == ((OP_LIT << 12) | 0));  // PUSH 0
    assert(output[1] == ((OP_JPC << 12) | 3));  // JPC to instruction 3
    assert(output[2] == ((OP_LIT << 12) | 2));  // PUSH 2
}

void test_file_assembly() {
    // Create test input file
    FILE* test_in = fopen("test.asm", "w");
    fprintf(test_in, "PUSH 42\nADD\nPUSH 10\n");
    fclose(test_in);

    // Test assembly
    int result = assemble_file("test.asm", "test.bin");
    assert(result == 3);  // Should generate 3 instructions

    // Verify output
    FILE* test_out = fopen("test.bin", "rb");
    uint16_t output[3];
    fread(output, sizeof(uint16_t), 3, test_out);
    fclose(test_out);

    assert(output[0] == (OP_LIT << OPCODE_SHIFT | 42));
    assert(output[1] == (OP_OPR << OPCODE_SHIFT | OPR_ADD));
    assert(output[2] == (OP_LIT << OPCODE_SHIFT | 10));
}

int main() {
    test_basic_stack_operation();
    test_add_operation();
    test_instruction_encoding();
    test_jump_operations();
    test_conditional_jump();
    test_file_assembly();
    printf("All tests passed!\n");
    return 0;
}