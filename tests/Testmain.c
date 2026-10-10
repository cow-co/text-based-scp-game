#include "unity.h"
#include "gameplay.h"
#include "menu.h"
#include <stdio.h>

void setUp() {}
void tearDown() {}

/**
 * Gameplay tests
 */

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

/**
 * Organisation Tests
 */

void test_BadOrganisationFiles() {
    Organisation org = {};
    int res = parse_organisation_data("tests/data/organisation/negative_budget.toml", &org);
    TEST_ASSERT(res == EOF);

    res = parse_organisation_data("tests/data/organisation/negative_rep.toml", &org);
    TEST_ASSERT(res == EOF);

    res = parse_organisation_data("tests/data/organisation/rep_over_100.toml", &org);
    TEST_ASSERT(res == EOF);

    res = parse_organisation_data("tests/data/organisation/too_many_skips.toml", &org);
    TEST_ASSERT(res == EOF);

    res = parse_organisation_data("tests/data/organisation/too_many_mtfs.toml", &org);
    TEST_ASSERT(res == EOF);

    res = parse_organisation_data("tests/data/organisation/skips_not_array.toml", &org);
    TEST_ASSERT(res == EOF);

    res = parse_organisation_data("tests/data/organisation/mtfs_not_array.toml", &org);
    TEST_ASSERT(res == EOF);

    res = parse_organisation_data("tests/data/organisation/nonexistent.toml", &org);
    TEST_ASSERT(res == EOF);
}

void test_GoodOrganisationFile() {
    Organisation org = {};
    int res = parse_organisation_data("tests/data/organisation/good.toml", &org);
    TEST_ASSERT(res == 0);
}

void test_InflictCasualties() {
    Personnel pers = {
        .operational = 10,
        .wounded = 1,
        .dead = 5
    };
    inflict_casualties(&pers, 2, 3);
    TEST_ASSERT_EQUAL_UINT16(7, pers.dead);
    TEST_ASSERT_EQUAL_UINT16(4, pers.wounded);
    TEST_ASSERT_EQUAL_UINT16(5, pers.operational);

    pers.operational = 10;
    pers.wounded = 1;
    pers.dead = 5;
    inflict_casualties(&pers, 11, 0);
    TEST_ASSERT_EQUAL_UINT16(15, pers.dead);
    TEST_ASSERT_EQUAL_UINT16(0, pers.operational);

    pers.operational = 10;
    pers.wounded = 1;
    pers.dead = 5;
    inflict_casualties(&pers, 0, 11);
    TEST_ASSERT_EQUAL_UINT16(11, pers.wounded);
    TEST_ASSERT_EQUAL_UINT16(0, pers.operational);

    // FIXME: Bug in the inflict_casualties; it doesn't check that deaths > operational prevents woundings
    pers.operational = 10;
    pers.wounded = 1;
    pers.dead = 5;
    inflict_casualties(&pers, 11, 11);
    TEST_ASSERT_EQUAL_UINT16(15, pers.dead);
    TEST_ASSERT_EQUAL_UINT16(1, pers.wounded);
    TEST_ASSERT_EQUAL_UINT16(0, pers.operational);
}

void test_RegeneratePersonnel() {
    Personnel pers = {
        .operational = 10,
        .wounded = 1,
        .dead = 5
    };
    regenerate(&pers, 1, true);
    TEST_ASSERT_EQUAL_UINT16(11, pers.operational);
    TEST_ASSERT_EQUAL_UINT16(0, pers.wounded);

    pers.operational = 10;
    pers.wounded = 1;
    pers.dead = 5;
    regenerate(&pers, 1, false);
    TEST_ASSERT_EQUAL_UINT16(11, pers.operational); 
    TEST_ASSERT_EQUAL_UINT16(1, pers.wounded);

    pers.operational = 10;
    pers.wounded = 1;
    pers.dead = 5;
    regenerate(&pers, 2, true);
    TEST_ASSERT_EQUAL_UINT16(11, pers.operational);
    TEST_ASSERT_EQUAL_UINT16(0, pers.wounded);
}


/**
 * Character Tests
 */

void test_BadPersonFiles() {

}

void test_BadSCPFiles() {

}

void test_BadMTFFiles() {

}

void test_GoodPersonFile() {

}

void test_GoodSCPFile() {

}

void test_GoodMTFFile() {

}


/**
 * Map Tests
 */

void test_BadMaps() {

}

void test_GoodMap() {

}


/**
 * Menu Tests
 */

void test_LoadBadFiles() {
    int res = load_game("tests/data/saves/nonexistent.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = load_game("tests/data/saves/bad.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);
}

void test_LoadGoodFile() {
    int res = load_game("tests/data/saves/good.toml");
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_UINT16(18, progress.org.reputation);
    TEST_ASSERT_EQUAL_UINT16(11, progress.currentRoom);
}


/**
 * Conversation Tests
 */

void test_BadConvos() {

}

void test_GoodConvos() {

}



int main() {
    UNITY_BEGIN();

    // Gameplay
    RUN_TEST(test_ChecksDefaultFlag);
    RUN_TEST(test_ChecksSetFlag);
    RUN_TEST(test_ChecksUnsetFlag);
    RUN_TEST(test_ChecksFlagSetAmongstMany);
    RUN_TEST(test_ChecksFlagUnsetAmongstMany);

    // Org
    RUN_TEST(test_BadOrganisationFiles);
    RUN_TEST(test_GoodOrganisationFile);
    RUN_TEST(test_InflictCasualties);
    RUN_TEST(test_RegeneratePersonnel);

    // Menu
    RUN_TEST(test_LoadBadFiles);
    RUN_TEST(test_LoadGoodFile);

    return UNITY_END();
}
