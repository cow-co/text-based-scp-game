#include "unity.h"
#include "gameplay.h"

void setUp() {}
void tearDown() {}

void test_ChecksDefaultFlag() {
    bool check = test_flag(1);
    TEST_ASSERT(check == false);
}

void test_ChecksSetFlag() {
    set_flag(1, true);
    bool check = test_flag(1);
    TEST_ASSERT(check == true);
    set_flag(1, false);
}

void test_ChecksUnsetFlag() {
    set_flag(1, true);
    set_flag(1, false);
    bool check = test_flag(1);
    TEST_ASSERT(check == false);
}

void test_ChecksFlagSetAmongstMany() {
    set_flag(1, true);
    set_flag(5, true);
    set_flag(10, true);
    set_flag(15, true);

    bool check = test_flag(5);

    TEST_ASSERT(check == true);
}

void test_ChecksFlagUnsetAmongstMany() {
    set_flag(1, true);
    set_flag(5, true);
    set_flag(10, true);
    set_flag(15, true);

    bool check = test_flag(6);

    TEST_ASSERT(check == false);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_ChecksDefaultFlag);
    RUN_TEST(test_ChecksSetFlag);
    RUN_TEST(test_ChecksUnsetFlag);
    RUN_TEST(test_ChecksFlagSetAmongstMany);
    RUN_TEST(test_ChecksFlagUnsetAmongstMany);
    return UNITY_END();
}
