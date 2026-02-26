# common variables
CC ?= cc
AR ?= ar
CFLAGS ?= -Wall -Wextra -Werror -pedantic -std=c11
CFLAGS += -Iinclude -g

BUILD_DIR = build

# all
all: lib

# lib build
SRC_DIR = src
LIB_DIR = $(BUILD_DIR)/lib
LIB = $(LIB_DIR)/libdds.a
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(LIB_DIR)/%.o)

lib: $(LIB)

$(LIB): $(OBJS)
	$(AR) rcs $@ $^

$(LIB_DIR)/%.o: $(SRC_DIR)/%.c | $(LIB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# test build
UNITY_DIR = external/unity/src
UNITY_SRC = $(UNITY_DIR)/unity.c

TEST_CFLAGS = $(CFLAGS) -I$(UNITY_DIR) -DUNITY_OUTPUT_COLOR -g

TEST_DIR = tests
TEST_OUT_DIR = $(BUILD_DIR)/tests
TEST_BIN = $(TEST_OUT_DIR)/tests
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(TEST_OUT_DIR)/%.o)
UNITY_OBJ = $(TEST_OUT_DIR)/unity.o

tests: $(TEST_BIN)

run_tests: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(LIB) $(TEST_OBJS) $(UNITY_OBJ)
	$(CC) $(TEST_CFLAGS) -o $@ $(TEST_OBJS) $(UNITY_OBJ) $(LIB)

$(TEST_OUT_DIR)/%.o: $(TEST_DIR)/%.c | $(TEST_OUT_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(UNITY_OBJ): $(UNITY_SRC) | $(TEST_OUT_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(TEST_OUT_DIR):
	mkdir -p $(TEST_OUT_DIR)

# clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean tests test examples
