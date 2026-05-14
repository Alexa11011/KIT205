#include <stdio.h>

#include "tests.h"

void run_all_tests(void) {
    printf("Running tests...\n");
    fflush(stdout);

    run_list_tests();
    run_hash_for_list_tests();
    run_relation_for_list_tests();
    run_avl_tests();
    run_hash_for_avl_tests();
    run_relation_for_avl_tests();

    printf("All tests passed.\n\n");
}
