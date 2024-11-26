`timescale 1ns/1ps
`include "pl0_opcodes.vh"

module tb_pl0_machine;
    reg clk = 0;
    reg reset = 1;          // Start in reset
    reg rst_n = 0;
    reg [7:0] char_in;        // Character input data
    reg char_in_valid;        // Input valid signal
    wire [7:0] char_out;      // Character output data
    wire char_out_valid;      // Output valid signal
    reg [15:0] instruction;   // Instruction input

    // Clock generation
    always #5 clk = ~clk;

    // Instantiate PL/0 machine
    pl0_machine uut (
        .clk(clk),
        .reset(reset),
        .instruction(instruction),
        .char_in(char_in),           // Keep char_in
        .char_in_valid(char_in_valid),
        .char_out(char_out),
        .char_out_valid(char_out_valid)
    );

    // Load program through memory interface
    task load_program;
        input [7:0] addr;
        input [7:0] data;
        begin
            @(posedge clk);
            uut.memory[addr] <= data;
            @(posedge clk);
        end
    endtask

    // Test tasks
    task test_push_and_add;
        begin
            // Use defined opcodes
            instruction = {`OP_LIT, 12'd42};  // PUSH 42
            #10;
            instruction = {`OP_LIT, 12'd58};  // PUSH 58
            #10;
            instruction = {`OP_OPR, 12'd0};   // ADD
        end
    endtask

    task test_shifts;
        begin
            // Test left shift
            instruction = {`OP_LIT, 12'd4};   // Push 4
            #10;
            instruction = {`OP_LIT, 12'd2};   // Push 2
            #10;
            instruction = {`OP_OPR, `OP_LSH}; // 4 << 2 = 16
            #10;

            // Test right shift
            instruction = {`OP_LIT, 12'd16};  // Push 16
            #10;
            instruction = {`OP_LIT, 12'd2};   // Push 2
            #10;
            instruction = {`OP_OPR, `OP_RSH}; // 16 >> 2 = 4
            #10;
        end
    endtask

    task test_comparisons;
        begin
            // Test LTE
            instruction = {`OP_LIT, 12'd5};   // Push 5
            #10;
            instruction = {`OP_LIT, 12'd5};   // Push 5
            #10;
            instruction = {`OP_OPR, `OP_LTE}; // 5 <= 5 = 1
            #10;
        end
    endtask

    // Test IO operations
    task test_io;
        begin
            // Test input
            char_in = 8'h41;         // ASCII 'A'
            char_in_valid = 1;
            instruction = {`OP_IN, 12'h0};
            #10;
            char_in_valid = 0;
            
            // Test output
            instruction = {`OP_OUT, 12'h0};
            #10;
        end
    endtask

    // Main test sequence
    initial begin
        $dumpfile("pl0_machine.vcd");
        $dumpvars(0, tb_pl0_machine);
        
        // Reset
        rst_n = 0;
        #10 rst_n = 1;
        
        // Release from reset after a few clocks
        #10 reset = 0;      // Release reset
        
        // Load test program
        load_program(0, `OP_LIT);
        load_program(1, 8'd42);
        load_program(2, `OP_LIT);
        load_program(3, 8'd58);
        
        // Run tests
        test_push_and_add();
        test_shifts();     // Add new test
        test_comparisons(); // Add new test
        
        // Check results
        #100;
        if ($test$plusargs("VERBOSE"))
            $display("Tests completed");
        
        $finish;
    end
endmodule

