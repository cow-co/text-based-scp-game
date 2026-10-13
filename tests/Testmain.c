#include "unity.h"
#include "gameplay.h"
#include "menu.h"
#include "map.h"
#include "characters.h"
#include "util.h"
#include "counts.h"
#include <stdio.h>
#include <string.h>

void setUp() {
    countChars = 100;
    countConvos = 100;
    countInteractables = 100;
    countMTFs = 100;
    countRooms = 100;
    countSCPs = 100;
}
void tearDown() {
}

void test_ArrayClear() {
    int blah[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    clear_array(sizeof(blah), &blah);
    const int expect[10] = {};
    TEST_ASSERT_EQUAL_INT_ARRAY(expect, blah, 10);

    SCP blahSCP[10] = {};
    strncpy(blahSCP[8].name, "testing", 8);
    TEST_ASSERT_EQUAL_STRING("testing", blahSCP[8].name);
    clear_array(sizeof(blahSCP), &blahSCP);
    TEST_ASSERT_EQUAL_STRING("", blahSCP[8].name);
}

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

    set_flag(0, true);
    check = test_flag(1);
    TEST_ASSERT(check == false);

    set_flag(1, true);
    check = test_flag(0);
    TEST_ASSERT(check == false);
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
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_organisation_data("tests/data/organisation/negative_rep.toml", &org);
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_organisation_data("tests/data/organisation/rep_over_100.toml", &org);
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_organisation_data("tests/data/organisation/too_many_skips.toml", &org);
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_organisation_data("tests/data/organisation/too_many_mtfs.toml", &org);
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_organisation_data("tests/data/organisation/skips_not_array.toml", &org);
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_organisation_data("tests/data/organisation/mtfs_not_array.toml", &org);
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_organisation_data("tests/data/organisation/nonexistent.toml", &org);
    TEST_ASSERT_EQUAL_INT(EOF, res);
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

void test_ChangingRep() {
    Organisation org = {};
    org.reputation = 50U;
    change_reputation(&org, -10);
    TEST_ASSERT_EQUAL_UINT8(40, org.reputation);

    org.reputation = 50U;
    change_reputation(&org, 10);
    TEST_ASSERT_EQUAL_UINT8(60, org.reputation);

    org.reputation = 50U;
    change_reputation(&org, 51);
    TEST_ASSERT_EQUAL_UINT8(100, org.reputation);

    org.reputation = 50U;
    change_reputation(&org, -51);
    TEST_ASSERT_EQUAL_UINT8(0, org.reputation);

    org.reputation = 50U;
    change_reputation(&org, 255);
    TEST_ASSERT_EQUAL_UINT8(100, org.reputation);
}

void test_ChangingBudget() {
    Organisation org = {};
    org.budget.totalAmount = 50000U;
    change_budget(&org, -10000);
    TEST_ASSERT_EQUAL_UINT(40000, org.budget.totalAmount);

    org.budget.totalAmount = 50000U;
    change_budget(&org, 10000);
    TEST_ASSERT_EQUAL_UINT(60000, org.budget.totalAmount);

    org.budget.totalAmount = 32767U;
    change_budget(&org, -32768);
    TEST_ASSERT_EQUAL_UINT(0, org.budget.totalAmount);

    org.budget.totalAmount = UINT_MAX;
    change_budget(&org, 1);
    TEST_ASSERT_EQUAL_UINT(UINT_MAX, org.budget.totalAmount);
}


/**
 * Character Tests
 */

void test_BadPersonFiles() {
    int res = parse_people("tests/data/characters/people/long_name.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_people("tests/data/characters/people/no_name.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_people("tests/data/characters/people/long_desc.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_people("tests/data/characters/people/long_role.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_people("tests/data/characters/people/aaaaaaaaaaaa.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);
}

void test_BadSCPFiles() {
    int res = parse_scps("tests/data/characters/scps/bad_class.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_scps("tests/data/characters/scps/no_class.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_scps("tests/data/characters/scps/no_name.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_scps("tests/data/characters/scps/long_name.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_scps("tests/data/characters/scps/long_desc.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_scps("tests/data/characters/scps/no_desc.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_scps("tests/data/characters/scps/aaaaaaaaaaaaa.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);
}

void test_BadMTFFiles() {
    int res = parse_mtf_data("tests/data/characters/mtfs/long_name.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_mtf_data("tests/data/characters/mtfs/no_name.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_mtf_data("tests/data/characters/mtfs/no_personnel.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_mtf_data("tests/data/characters/mtfs/negative_personnel.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_mtf_data("tests/data/characters/mtfs/aaaaaaaaaaaa.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);
}

void test_GoodPersonFile() {
    int res = parse_people("tests/data/characters/people/good_people.toml");
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_STRING("Alto Clef", allPeople[0].name);
    TEST_ASSERT_EQUAL_STRING("Director of Site 19", allPeople[0].role);
    TEST_ASSERT_EQUAL_STRING("A bit of an arsehole", allPeople[0].description);
    TEST_ASSERT_EQUAL_INT(2, allPeople[0].conversation);

    res = parse_people("tests/data/characters/people/good_no_optionals.toml");
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_STRING("Alto Clefy", allPeople[0].name);
}

void test_GoodSCPFile() {
    int res = parse_scps("tests/data/characters/scps/good_scps.toml");
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_STRING("SCP-049", allSCPs[0].name);
    TEST_ASSERT_EQUAL_STRING("SCP-682", allSCPs[1].name);
    TEST_ASSERT_EQUAL_STRING("Resilient Lizard", allSCPs[1].description);
    TEST_ASSERT_EQUAL_INT(2, allSCPs[1].objectClass);
}

void test_GoodMTFFile() {
    int res = parse_mtf_data("tests/data/characters/mtfs/good_mtfs.toml");
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_STRING("Nine-Tailed Fox", allMTFs[0].name);
    TEST_ASSERT_EQUAL_STRING("Red Right Hand", allMTFs[1].name);
    TEST_ASSERT_EQUAL_INT(24, allMTFs[1].personnel.operational);
}


/**
 * Map Tests
 */

void test_BadMaps() {
    int res = parse_facility("tests/data/facility/long_name.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_facility("tests/data/facility/no_name.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_facility("tests/data/facility/long_desc.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_facility("tests/data/facility/no_desc.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_facility("tests/data/facility/too_many_chars.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_facility("tests/data/facility/negative_char_id.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_facility("tests/data/facility/bad_scp_id.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_facility("tests/data/facility/too_many_connections.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_facility("tests/data/facility/bad_connection_id.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_facility("tests/data/facility/bad_flag_id.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_facility("tests/data/facility/aaaaa.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);
}

void test_GoodMaps() {
    int res = parse_facility("tests/data/facility/good_all.toml");
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(1, siteFacility.entranceRooms[0]);
    TEST_ASSERT_EQUAL_STRING("Entryway", siteFacility.rooms[0].name);
    TEST_ASSERT_EQUAL_STRING("test", siteFacility.rooms[0].description);
    TEST_ASSERT_EQUAL_INT(2, siteFacility.rooms[0].connections[0]);
    TEST_ASSERT_EQUAL_INT(2, siteFacility.rooms[0].characters[0]);
    TEST_ASSERT_EQUAL_INT(1, siteFacility.rooms[0].requiresFlagUnset);
    TEST_ASSERT_EQUAL_INT(10, siteFacility.rooms[0].requiresFlagSet);
    TEST_ASSERT_EQUAL_INT(1, siteFacility.rooms[0].firstEntrySetsFlag);
    TEST_ASSERT_EQUAL_INT(10, siteFacility.rooms[0].firstEntryUnsetsFlag);
    TEST_ASSERT_EQUAL_INT(1, siteFacility.rooms[0].containedSCP);

    res = parse_facility("tests/data/facility/good_no_optionals.toml");
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(2, siteFacility.entranceRooms[1]);
    TEST_ASSERT_EQUAL_STRING("Entryway", siteFacility.rooms[0].name);
    TEST_ASSERT_EQUAL_STRING("test", siteFacility.rooms[0].description);
    TEST_ASSERT_EQUAL_INT(1, siteFacility.rooms[0].connections[0]);
}

void test_BadInteractablesFiles() {
    int res = parse_interactables("tests/data/facility/interactables/long_name.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_interactables("tests/data/facility/interactables/no_name.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_interactables("tests/data/facility/interactables/no_inspect.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_interactables("tests/data/facility/interactables/long_inspect.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_interactables("tests/data/facility/interactables/no_interact.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_interactables("tests/data/facility/interactables/long_interact.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_interactables("tests/data/facility/interactables/bad_flag_id.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);
}

void test_GoodInteractablesFiles() {
    int res = parse_interactables("tests/data/facility/interactables/good_all.toml");
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(1, allInteractables[0].requiresFlag);
    TEST_ASSERT_EQUAL_INT(1, allInteractables[0].unrequiresFlag);
    TEST_ASSERT_EQUAL_INT(1, allInteractables[0].setsFlag);
    TEST_ASSERT_EQUAL_INT(1, allInteractables[0].unsetsFlag);
    TEST_ASSERT_EQUAL_STRING("test", allInteractables[0].name);
    TEST_ASSERT_EQUAL_STRING("inspect", allInteractables[0].inspectText);
    TEST_ASSERT_EQUAL_STRING("interact", allInteractables[0].interactText);

    res = parse_interactables("tests/data/facility/interactables/good_no_optionals.toml");
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_STRING("test", allInteractables[0].name);
    TEST_ASSERT_EQUAL_STRING("inspect", allInteractables[0].inspectText);
    TEST_ASSERT_EQUAL_STRING("interact", allInteractables[0].interactText);
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
    int res = parse_convos("tests/data/convo/bad_flag.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_convos("tests/data/convo/bad_link.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_convos("tests/data/convo/long_option.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_convos("tests/data/convo/long_text.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_convos("tests/data/convo/no_link.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_convos("tests/data/convo/no_text.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_convos("tests/data/convo/textless_option.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);

    res = parse_convos("tests/data/convo/nonexistent.toml");
    TEST_ASSERT_EQUAL_INT(EOF, res);
}

void test_GoodConvos() {
    int res = parse_convos("tests/data/convo/good_all.toml");
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_STRING("test", allConvos[0].nodes[0].text);
    TEST_ASSERT_EQUAL_INT(10, allConvos[0].nodes[0].requiresFlagSet);
    TEST_ASSERT_EQUAL_INT(1, allConvos[0].nodes[0].requiresFlagUnset);
    TEST_ASSERT_EQUAL_INT(1, allConvos[0].nodes[0].setsFlag);
    TEST_ASSERT_EQUAL_INT(10, allConvos[0].nodes[0].unsetsFlag);
    TEST_ASSERT_EQUAL_STRING("testing", allConvos[0].nodes[0].options[0].text);
    TEST_ASSERT_EQUAL_INT(2, allConvos[0].nodes[0].options[0].linksTo);

    res = parse_convos("tests/data/convo/good_no_optionals.toml");
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_STRING("testy", allConvos[0].nodes[0].text);
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
    RUN_TEST(test_ChangingBudget);
    RUN_TEST(test_ChangingRep);

    // Characters
    RUN_TEST(test_BadMTFFiles);
    RUN_TEST(test_GoodMTFFile);
    RUN_TEST(test_BadPersonFiles);
    RUN_TEST(test_GoodPersonFile);
    RUN_TEST(test_BadSCPFiles);
    RUN_TEST(test_GoodSCPFile);

    // Map
    RUN_TEST(test_BadMaps);
    RUN_TEST(test_GoodMaps);
    RUN_TEST(test_BadInteractablesFiles);
    RUN_TEST(test_GoodInteractablesFiles);

    // Menu
    RUN_TEST(test_LoadBadFiles);
    RUN_TEST(test_LoadGoodFile);

    // Conversations
    RUN_TEST(test_BadConvos);
    RUN_TEST(test_GoodConvos);

    // Misc
    RUN_TEST(test_ArrayClear);

    return UNITY_END();
}
