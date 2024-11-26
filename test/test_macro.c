#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_macro_functionality(void) {
    CU_ASSERT(1 + 1 == 2);
    CU_ASSERT(2 * 2 == 4);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Macro Test Suite", 0, 0);
    CU_add_test(suite, "Test Macro Functionality", test_macro_functionality);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}