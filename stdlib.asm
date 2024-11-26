; stdlib.asm - Standard macro library for PL/0 assembly

; Stack operations
%macro PUSH 1
    LIT %1          ; Push literal onto stack
%endmacro

%macro POP 0
    STO 0           ; Pop value from stack
%endmacro

; Arithmetic helpers
%macro ADD2 0
    ADD             ; Add top two stack values
%endmacro

%macro SUB2 0
    SUB             ; Subtract top two stack values
%endmacro

%macro MUL2 0
    MUL             ; Multiply top two stack values
%endmacro

%macro DIV2 0
    DIV             ; Divide top two stack values
%endmacro

%macro MOD2 0
    MOD             ; Modulo top two stack values
%endmacro

; Function calling
%macro CALL 2
    PUSH %2         ; Push parameter count
    CAL %1          ; Call function
%endmacro

; Comparison macros
%macro EQ 0
    EQL             ; Equal
%endmacro

%macro NEQ 0
    NEQ             ; Not equal
%endmacro

%macro LT 0
    LSS             ; Less than
%endmacro

%macro GTE 0
    GTE             ; Greater than or equal
%endmacro

%macro GT 0
    GTR             ; Greater than
%endmacro

%macro LTE 0
    LTE             ; Less than or equal
%endmacro

; Bitwise operations
%macro LSH 0
    LSH             ; Left shift
%endmacro

%macro RSH 0
    RSH             ; Right shift
%endmacro

; I/O operations
%macro INPUT 0
    IN              ; Read input
%endmacro

%macro OUTPUT 0
    OUT             ; Write output
%endmacro

; Control flow
%macro JUMPZ 1
    JPC %1          ; Jump to label if zero
%endmacro

%macro RJUMPZ 1
    RJPC %1         ; Relative jump if zero
%endmacro

%macro JUMP 1
    JMP %1          ; Unconditional jump
%endmacro

%macro CALL 1
    CAL %1          ; Call subroutine
%endmacro

%macro RET 0
    RET             ; Return from subroutine
%endmacro