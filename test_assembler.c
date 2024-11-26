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
    assert(output[2] == (OPR_ADD << 12));
}

void test_instruction_encoding() {
    // Test that LIT instruction correctly encodes value
    assert((OP_LIT << 12 | 42) == 0x102A);  // 0001 0000 0010 1010
    
    // Test that stack operation has no operand
    assert((OPR_ADD << 12) == 0x2000);       // 0010 0000 0000 0000
    
    printf("Instruction encoding tests passed\n");
}

void test_jump_operations() {
    const char* input = 
        "start:\n"
        "PUSH 1\n"
        "JPC end\n"
        "PUSH 2\n"
        "end:\n"
        "PUSH 3\n";
        
    uint16_t output[5];
    int result = assemble_string(input, output, 5);
    
    assert(result == 4);
    assert(output[0] == ((OP_LIT << 12) | 1));
    assert(output[1] == ((OP_JPC << 12) | 3));
    assert(output[2] == ((OP_LIT << 12) | 2));
    assert(output[3] == ((OP_LIT << 12) | 3));
}

int main() {
    test_basic_stack_operation();
    test_add_operation();
    test_instruction_encoding();
    test_jump_operations();
    printf("All tests passed!\n");
    return 0;
}