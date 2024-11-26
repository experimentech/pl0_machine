// test_macro.c
#include <assert.h>
#include "macro.h"

void test_push_macro() {
    MacroTable table = {0};
    const char* macro_def = "%macro PUSH 1\n"
                           "LIT %1\n"
                           "%endmacro\n";
    
    process_macro(macro_def, &table);
    assert(table.count == 1);
    
    char* args[] = {"42"};
    char* expanded = expand_macro("PUSH", args, 1, &table);
    assert(strcmp(expanded, "LIT 42") == 0);
    free(expanded);
}

int main() {
    test_push_macro();
    printf("All macro tests passed!\n");
    return 0;
}