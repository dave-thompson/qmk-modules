# Makefile for Unity tests
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iunity

# Source files
UNITY_SRC = unity/unity.c
UTILS_SRC = ../lumberjack_utils.c
TEST_SRC = test_lumberjack_utils.c

# Output binary
TEST_BINARY = test_runner

.PHONY: test clean all test-keep

# Default target
all: $(TEST_BINARY)

# Build and run tests, then clean up
test: $(TEST_BINARY)
	@echo "Running tests..."
	./$(TEST_BINARY)
	@$(MAKE) clean --no-print-directory

# Build test binary
$(TEST_BINARY): $(TEST_SRC) $(UTILS_SRC) $(UNITY_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Clean up
clean:
	rm -f $(TEST_BINARY)