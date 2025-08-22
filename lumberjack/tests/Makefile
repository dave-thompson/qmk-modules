# Makefile for Unity tests
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iunity

# Source files
UNITY_SRC = unity/unity.c
UTILS_SRC = ../lumberjack_utils.c
COLORS_SRC = ../lumberjack_colors.c
TEST_UTILS_SRC = test_lumberjack_utils.c
TEST_COLORS_SRC = test_lumberjack_colors.c

# Output binaries
TEST_UTILS_BINARY = test_utils_runner
TEST_COLORS_BINARY = test_colors_runner

.PHONY: test clean all test-keep test-utils test-colors

# Default target - run all tests
all: test

# Build and run all tests, then clean up
test: test-utils test-colors
	@$(MAKE) clean --no-print-directory

# Build and run utils tests
test-utils: $(TEST_UTILS_BINARY)
	@echo "Running lumberjack_utils tests..."
	./$(TEST_UTILS_BINARY)

# Build and run colors tests
test-colors: $(TEST_COLORS_BINARY)
	@echo "Running lumberjack_colors tests..."
	./$(TEST_COLORS_BINARY)

# Build utils test binary
$(TEST_UTILS_BINARY): $(TEST_UTILS_SRC) $(UTILS_SRC) $(UNITY_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Build colors test binary
$(TEST_COLORS_BINARY): $(TEST_COLORS_SRC) $(COLORS_SRC) $(UNITY_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Clean up
clean:
	rm -f $(TEST_UTILS_BINARY) $(TEST_COLORS_BINARY)