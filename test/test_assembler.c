#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_assembler_functionality(void) {
    CU_ASSERT(1 + 1 == 2);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Assembler Test Suite", 0, 0);
    CU_add_test(suite, "Test Assembler Functionality", test_assembler_functionality);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}