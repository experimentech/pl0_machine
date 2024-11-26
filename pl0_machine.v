`timescale 1ns / 1ps

module pl0_machine (
    input wire clk,
    input wire reset,        
    input wire [7:0] char_in,
    input wire char_in_valid,
    output reg [7:0] char_out,
    output reg char_out_valid
);

    // Program Counter
    reg [15:0] pc;

    // Memory for instructions and data
    reg [7:0] memory [0:65535];

    // Instruction Register
    reg [7:0] ir;

    // Stack
    reg [15:0] stack [0:255];
    reg [7:0] sp; // Stack pointer

    // FIFO for character input
    reg [7:0] fifo [0:15]; // Small FIFO buffer
    reg [3:0] fifo_wp; // FIFO write pointer
    reg [3:0] fifo_rp; // FIFO read pointer
    reg fifo_empty;
    reg fifo_full;

    // Intermediate nets for debug
    wire [15:0] stack_top;
    assign stack_top = (sp > 0) ? stack[sp - 1] : 16'hXXXX; // Ensure valid indexing

    // Registers
    reg [7:0] reg_b;

    // State machine for program flow control
    reg [1:0] state;
    parameter FETCH = 2'b00, DECODE = 2'b01, EXECUTE = 2'b10;

    // Opcodes
    localparam LIT = 8'h00;
    localparam OPR = 8'h01;
    localparam LOD = 8'h02;
    localparam STO = 8'h03;
    localparam CAL = 8'h04;
    localparam RET = 8'h05;
    localparam JMP = 8'h06;
    localparam JPC = 8'h07;
    localparam IN = 8'h08;  // New opcode for input
    localparam OUT = 8'h09; // New opcode for output
    localparam RJMP = 8'h0A;
    localparam RJPC = 8'h0B;


    // Initialize memory and FIFO
    integer i;
    initial begin
        for (i = 0; i < 65536; i = i + 1) begin
            memory[i] = 8'h00;
        end
        fifo_wp = 0;
        fifo_rp = 0;
        fifo_empty = 1;
        fifo_full = 0;
    end

    // Main state machine
    always @(posedge clk or posedge reset) begin
        if (reset) begin     // Reset is active HIGH
            pc <= 0;
            sp <= 0;
            char_out_valid <= 0;
            char_out <= 8'h00;
            fifo_wp <= 0;
            fifo_rp <= 0;
            fifo_empty <= 1;
            fifo_full <= 0;
            state <= FETCH;
        end else begin
            // Handle FIFO input
            if (char_in_valid && !fifo_full) begin
                fifo[fifo_wp] <= char_in;
                fifo_wp <= fifo_wp + 1;
                fifo_empty <= 0;
                if (fifo_wp == 15)
                    fifo_full <= 1;
            end

            case (state)
                FETCH: begin
                    // Fetch instruction
                    ir <= memory[pc]; // Load instruction into IR
                    pc <= pc + 1;     // Increment PC
                    state <= DECODE;
                end

                DECODE: begin
                    // Decode instruction
                    case (ir)
                        LIT, OPR, LOD, STO, CAL, RET, JMP, JPC, IN, OUT, RJMP, RJPC: state <= EXECUTE;
                        default: state <= FETCH;
                    endcase
                end
                EXECUTE: begin
                    // Execute instruction
                    case (ir)
                        LIT: begin
                            stack[sp] <= memory[pc + 1];
                            sp <= sp + 1;
                            pc <= pc + 2; // Increment PC by 2 (for instruction and its argument)
                        end
                        OPR: begin
                            // Check if the stack has enough values
                            if (sp > 1) begin
                                // Operation logic
                                case (memory[pc + 1])
                                    8'h00: begin // Return
                                        sp <= sp - 1;
                                        pc <= stack[sp];
                                    end
                                    8'h01: stack[sp-2] <= stack[sp-2] + stack[sp-1]; // ADD
                                    8'h02: stack[sp-2] <= stack[sp-2] - stack[sp-1]; // SUB
                                    8'h03: stack[sp-2] <= stack[sp-2] * stack[sp-1]; // MUL
                                    8'h04: stack[sp-2] <= stack[sp-2] / stack[sp-1]; // DIV
                                    8'h05: stack[sp-2] <= stack[sp-2] % stack[sp-1]; // MOD
                                    8'h06: stack[sp-2] <= stack[sp-2] == stack[sp-1]; // EQL
                                    8'h07: stack[sp-2] <= stack[sp-2] != stack[sp-1]; // NEQ
                                    8'h08: stack[sp-2] <= stack[sp-2] < stack[sp-1];  // LSS
                                    8'h09: stack[sp-2] <= stack[sp-2] >= stack[sp-1]; // GTE
                                    8'h0A: stack[sp-2] <= stack[sp-2] > stack[sp-1];  // GTR
                                    8'h0B: stack[sp-2] <= stack[sp-2] <= stack[sp-1]; // LTE
                                    8'h0C: stack[sp-2] <= stack[sp-2] << stack[sp-1]; // LSH
                                    8'h0D: stack[sp-2] <= stack[sp-2] >> stack[sp-1]; // RSH
                                endcase
                                sp <= sp - 1;
                                pc <= pc + 2; // Increment PC by 2 (for instruction and its argument)
                            end else begin
                                // Handle underflow case (reset or error)
                                state <= FETCH;
                            end
                        end
                        LOD: begin
                            stack[sp] <= memory[stack[sp-1]];
                            sp <= sp + 1;
                            pc <= pc + 2; // Increment PC by 2 (for instruction and its argument)
                        end
                        STO: begin
                            if (sp > 1) begin
                                memory[stack[sp-1]] <= stack[sp-2];
                                sp <= sp - 2;
                                pc <= pc + 2; // Increment PC by 2 (for instruction and its argument)
                            end else begin
                                // Handle stack underflow error
                                state <= FETCH;
                            end
                        end
                        CAL: begin
                            stack[sp] <= pc + 2;
                            sp <= sp + 1;
                            pc <= memory[pc + 1]; // Set PC to the address in the current memory location
                        end
                        RET: begin
                            sp <= sp - 1;
                            pc <= stack[sp];
                        end
                        JMP: begin
                            pc <= memory[pc]; // Set PC to the address in the current memory location
                        end
                        JPC: begin
                            if (stack[sp-1] == 0) begin
                                pc <= memory[pc]; // Set PC to the address in the current memory location
                            end else begin
                                pc <= pc + 2; // Increment PC by 2 (for instruction and its argument)
                            end
                            sp <= sp - 1;
                        end
                        IN: begin
                            // Ensure FIFO has valid data
                            if (!fifo_empty && fifo_rp != fifo_wp) begin
                                stack[sp] <= fifo[fifo_rp];
                                sp <= sp + 1;
                                fifo_rp <= fifo_rp + 1;
                                if (fifo_rp == fifo_wp)
                                    fifo_empty <= 1;
                                fifo_full <= 0;
                                pc <= pc + 1; // Move to next instruction only if valid data was processed
                            end else begin
                                // Wait for valid data
                                state <= FETCH;
                            end
                        end
                        OUT: begin
                            // Ensure the stack pointer is decremented after reading
                            if (sp > 0) begin
                                char_out <= stack[sp - 1]; // Output character from stack
                                sp <= sp - 1;
                                char_out_valid <= 1; // Indicate output is valid
                            end else begin
                                // Handle underflow case (reset or error)
                                char_out_valid <= 0;
                            end
                            pc <= pc + 1; // Increment PC by 1 (for the instruction)
                        end
                        RJMP: begin
                            pc <= pc + $signed(memory[pc + 1]) + 2; // Add offset + instruction length
                        end
                        RJPC: begin
                            if (stack[sp - 1] == 0) begin
                                pc <= pc + $signed(memory[pc + 1]) + 2; // Add offset + instruction length
                            end else begin
                                pc <= pc + 2; // Skip to next instruction
                            end
                            sp <= sp - 1;
                        end

                    endcase
                    state <= FETCH;
                end
            endcase
        end
    end

    // Simple debug output with stack contents
    initial begin
        $monitor("Time: %0t | pc = %0d | state = %0b | sp = %0d | stack_top = %h | char_in = %h | char_out = %h | ir = %h | char_out_valid = %b",
                 $time, pc, state, sp, stack_top, char_in, char_out, ir, char_out_valid);
    end

endmodule


