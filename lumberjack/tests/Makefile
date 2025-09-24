# Makefile for Unity tests
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iunity

# Source files
UNITY_SRC = unity/unity.c
UTILS_SRC = ../lumberjack_utils.c
COLOR_QUEUE_SRC = ../lumberjack_color_queue.c
TEST_UTILS_SRC = test_lumberjack_utils.c
TEST_COLOR_QUEUE_SRC = test_lumberjack_color_queue.c

# Output binaries
TEST_UTILS_BINARY = test_utils_runner
TEST_COLOR_QUEUE_BINARY = test_color_queue_runner

.PHONY: test clean all test-keep test-utils test-color-queue

# Default target - run all tests
all: test

# Build and run all tests, then clean up
test: test-utils test-color-queue
	@$(MAKE) clean --no-print-directory

# Build and run utils tests
test-utils: $(TEST_UTILS_BINARY)
	@echo "Running lumberjack_utils tests..."
	./$(TEST_UTILS_BINARY)

# Build and run colors tests
test-color-queue: $(TEST_COLOR_QUEUE_BINARY)
	@echo "Running lumberjack_color_queue tests..."
	./$(TEST_COLOR_QUEUE_BINARY)

# Build utils test binary
$(TEST_UTILS_BINARY): $(TEST_UTILS_SRC) $(UTILS_SRC) $(UNITY_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Build color queue test binary
$(TEST_COLOR_QUEUE_BINARY): $(TEST_COLOR_QUEUE_SRC) $(COLOR_QUEUE_SRC) $(UNITY_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Clean up
clean:
	rm -f $(TEST_UTILS_BINARY) $(TEST_COLOR_QUEUE_BINARY)