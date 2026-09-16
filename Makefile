SRC_DIR := src
INC_DIR := include
#LIB_DIR := lib
OBJ_DIR := obj
BIN_DIR := bin

EXE := $(BIN_DIR)/scp

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS := -Iinclude -MMD -MP
CFLAGS	 := -Wall --std=c2x
#LDFLAGS  := -Llib
LDLIBS	 := -lm

# Targets
all: $(EXE)
.PHONY: all clean

$(BIN_DIR) $(OBJ_DIR): 
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(EXE): $(OBJ)
	$(CC) $^ $(LDLIBS) -o $@

clean: 
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)
