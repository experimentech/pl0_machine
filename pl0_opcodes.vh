// pl0_opcodes.vh
`define OP_LIT 4'h0    // Load literal
`define OP_OPR 4'h1    // Arithmetic operation
`define OP_LOD 4'h2    // Load from memory
`define OP_STO 4'h3    // Store to memory
`define OP_CAL 4'h4    // Call subroutine
`define OP_IN  4'h8    // Input operation
`define OP_OUT 4'h9    // Output operation

// Additional arithmetic operations for OPR instruction
`define OP_LSH  8'h0C  // Left shift
`define OP_RSH  8'h0D  // Right shift
`define OP_LTE  8'h0B  // Less than or equal