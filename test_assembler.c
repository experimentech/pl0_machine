// test_assembler.c
#include <stdio.h>
#include <assert.h>
#include "assembler.h"

void test_basic_stack_operation() {
    const char* input = "LIT 42\n";
    uint16_t output[1];
    int result = assemble_string(input, output, 1);
    assert(result == 1);
    assert(output[0] == ((OP_LIT << 12) | 42));
}

void test_add_operation() {
    const char* input = "LIT 5\nLIT 3\nADD\n";  // Changed PUSH to LIT
    uint16_t output[3];
    int result = assemble_string(input, output, 3);
    assert(result == 3);
    assert(output[0] == ((OP_LIT << 12) | 5));
    assert(output[1] == ((OP_LIT << 12) | 3));
    assert(output[2] == ((OP_OPR << 12) | OPR_ADD));
}

void test_instruction_encoding() {
    // Test LIT instruction - corrected expectation
    assert((OP_LIT << 12 | 42) == 0x002A);  // 0x0 << 12 | 0x2A
    
    // Test ADD operation - using OP_OPR (0x1)
    assert((OP_OPR << 12 | OPR_ADD) == 0x1001);
}

void test_jump_operations() {
    const char* input = 
        "start:\n"
        "LIT 1\n"      // Changed PUSH to LIT
        "JMP end\n"
        "LIT 2\n"      // Changed PUSH to LIT
        "end:\n"
        "LIT 3\n";     // Changed PUSH to LIT
        
    uint16_t output[4];
    int result = assemble_string(input, output, 4);
    
    assert(result == 3);
    assert(output[0] == ((OP_LIT << 12) | 1));  // PUSH 1
    assert(output[1] == ((OP_JMP << 12) | 2));  // JMP to instruction 2 (PUSH 3)
    assert(output[2] == ((OP_LIT << 12) | 3));  // PUSH 3
}

void test_conditional_jump() {
    const char* input = 
        "LIT 0\n"      // instruction 0
        "JPC skip\n"   // instruction 1 
        "LIT 1\n"      // instruction 2
        "skip:\n"      // points to address 3
        "LIT 2\n";     // instruction 3

    uint16_t output[4];
    int result = assemble_string(input, output, 4);
    
    // Expect 4 instructions total
    assert(result == 4);
    assert(output[0] == ((OP_LIT << 12) | 0));
    assert(output[1] == ((OP_JPC << 12) | 3));  // Jump to address 3 if zero
    assert(output[2] == ((OP_LIT << 12) | 1));
    assert(output[3] == ((OP_LIT << 12) | 2));
}

void test_file_assembly() {
    // Create test input file
    FILE* test_in = fopen("test.asm", "w");
    fprintf(test_in, "LIT 42\nADD\nLIT 10\n");  // Changed PUSH to LIT
    fclose(test_in);

    // Test assembly
    int result = assemble_file("test.asm", "test.bin");
    assert(result == 3);

    // Verify output
    FILE* test_out = fopen("test.bin", "rb");
    uint16_t output[3];
    fread(output, sizeof(uint16_t), 3, test_out);
    fclose(test_out);

    // Verify generated instructions
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