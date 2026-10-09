SRC_DIR := src
TEST_DIR := tests
OBJ_DIR := obj
BIN_DIR := bin
RESULTS_DIR := results

EXE := $(BIN_DIR)/scpgame
TEST_EXE := $(BIN_DIR)/test
SRC := $(wildcard $(SRC_DIR)/*.c)
TEST_SRC := $(wildcard $(TEST_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_OBJ := $(TEST_SRC:$(TEST_DIR)/%.c=$(OBJ_DIR)/%.o) $(OBJ_DIR)/unity.o

CC := gcc
CPPFLAGS := -Iinclude -MMD -MP
TESTCPPFLAGS := -Iinclude -Iunity/src -MMD -MP
CFLAGS	 := -Wall -std=c23 -pedantic
LDFLAGS	 := -Llib
LDLIBS	 := -lm

IGNORED = `grep -s ":IGNORE" $(RESULTS_DIR)/*.txt | wc -l`
FAILED = `grep -s ":FAIL" $(RESULTS_DIR)/*.txt | wc -l`
PASSED = `grep -s ":PASS" $(RESULTS_DIR)/*.txt | wc -l`

.PHONY: all clean test


all: $(EXE)

test: $(TEST_EXE)
	@echo "-----------------------\nIGNORED: $(IGNORED)\n-----------------------"
	@echo "-----------------------\nFAILED: $(FAILED)\n-----------------------"
	@echo "-----------------------\nPASSED: $(PASSED)\n-----------------------"

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(TEST_EXE): $(filter-out obj/main.o, $(OBJ)) $(TEST_OBJ) | $(BIN_DIR) $(RESULTS_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	$@ > $(RESULTS_DIR)/test_results.txt 2>&1

$(OBJ_DIR)/Test%.o: $(TEST_DIR)/Test%.c | $(OBJ_DIR)
	$(CC) $(TESTCPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/unity.o: unity/src/unity.c | $(OBJ_DIR)
	$(CC) $(TESTCPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR) $(RESULTS_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)
