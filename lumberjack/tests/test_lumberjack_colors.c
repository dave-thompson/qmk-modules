#define LUMBERJACK_MAX_COLORS 12

#include "unity.h"
#include "../lumberjack_colors.h"
#include <string.h>


void setUp(void) {
    lumberjack_reset_colors();
}

void tearDown(void) {
    lumberjack_reset_colors();
}

void test_empty_queue_returns_empty_string(void) {
    const char* result = lumberjack_get_next_color();
    TEST_ASSERT_EQUAL_STRING("", result);
}

void test_single_color_add_and_get(void) {
    const char* test_color = "\033[91m";
    
    lumberjack_release_color(test_color);
    const char* result = lumberjack_get_next_color();
    
    TEST_ASSERT_EQUAL_STRING(test_color, result);
}

void test_single_color_queue_becomes_empty_after_get(void) {
    const char* test_color = "\033[91m";
    
    lumberjack_release_color(test_color);
    lumberjack_get_next_color();  // Remove the item
    
    const char* result = lumberjack_get_next_color();
    TEST_ASSERT_EQUAL_STRING("", result);
}

void test_multiple_colors_fifo_order(void) {
    const char* color1 = "\033[91m";  // Bright Red
    const char* color2 = "\033[92m";  // Bright Green
    const char* color3 = "\033[94m";  // Bright Blue
    
    lumberjack_release_color(color1);
    lumberjack_release_color(color2);
    lumberjack_release_color(color3);
    
    TEST_ASSERT_EQUAL_STRING(color1, lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING(color2, lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING(color3, lumberjack_get_next_color());
}

void test_queue_empty_after_removing_all_items(void) {
    const char* color1 = "\033[91m";
    const char* color2 = "\033[92m";
    
    lumberjack_release_color(color1);
    lumberjack_release_color(color2);
    
    lumberjack_get_next_color();  // Remove first
    lumberjack_get_next_color();  // Remove second
    
    const char* result = lumberjack_get_next_color();
    TEST_ASSERT_EQUAL_STRING("", result);
}

void test_queue_wraparound(void) {

    // Create test colors
    const char* test_colors[LUMBERJACK_MAX_COLORS];
    for (int i = 0; i < LUMBERJACK_MAX_COLORS; i++) {
        static char color_buffers[LUMBERJACK_MAX_COLORS][10];
        sprintf(color_buffers[i], "\033[%dm", 30 + i);
        test_colors[i] = color_buffers[i];
    }

    const char* extra_test_colors[2];
    for (int i = 0; i < 2; i++) {
        static char color_buffers[2][10];
        sprintf(color_buffers[i], "\033[%dm", 10 + i);
        extra_test_colors[i] = color_buffers[i];
    }
    
    // Fill queue to capacity
    for (int i = 0; i < LUMBERJACK_MAX_COLORS; i++) {
        lumberjack_release_color(test_colors[i]);
    }
    
    // Remove three
    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL_STRING(test_colors[i], lumberjack_get_next_color());
    }
    
    // Add the extras
    for (int i = 0; i < 2; i++) {
        lumberjack_release_color(extra_test_colors[i]);
    }
    
    // Verify remaining originals
    for (int i = 0; i < LUMBERJACK_MAX_COLORS - 3; i++) {
        TEST_ASSERT_EQUAL_STRING(test_colors[i+3], lumberjack_get_next_color());
    }
    
    // Verify extras
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_EQUAL_STRING(extra_test_colors[i], lumberjack_get_next_color());
    }
}

void test_queue_full_ignores_additional_items(void) {
    const char* test_colors[LUMBERJACK_MAX_COLORS + 2];
    
    // Create test color strings
    for (int i = 0; i < LUMBERJACK_MAX_COLORS + 2; i++) {
        static char color_buffers[LUMBERJACK_MAX_COLORS + 2][10];
        sprintf(color_buffers[i], "\033[%dm", 30 + i);
        test_colors[i] = color_buffers[i];
    }
    
    // Fill queue beyond capacity
    for (int i = 0; i < LUMBERJACK_MAX_COLORS + 2; i++) {
        lumberjack_release_color(test_colors[i]);
    }
    
    // Should only get the first LUMBERJACK_MAX_COLORS items
    for (int i = 0; i < LUMBERJACK_MAX_COLORS; i++) {
        TEST_ASSERT_EQUAL_STRING(test_colors[i], lumberjack_get_next_color());
    }
    
    // Queue should now be empty
    TEST_ASSERT_EQUAL_STRING("", lumberjack_get_next_color());
}

void test_LUMBERJACK_COLOR_CODES_macro(void) {
    LUMBERJACK_COLOR_CODES(
        "\033[91m",  // Bright Red
        "\033[92m",  // Bright Green  
        "\033[94m"   // Bright Blue
    );
    
    TEST_ASSERT_EQUAL_STRING("\033[91m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[92m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[94m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("", lumberjack_get_next_color());  // Should be empty now
}

void test_LUMBERJACK_COLOR_CODES_macro_large_set(void) {
    LUMBERJACK_COLOR_CODES(
        "\033[91m",  // Bright Red
        "\033[92m",  // Bright Green  
        "\033[94m",  // Bright Blue
        "\033[93m",  // Bright Yellow
        "\033[95m",  // Bright Magenta
        "\033[96m",  // Bright Cyan
        "\033[31m",  // Red
        "\033[35m",  // Magenta
        "\033[33m",  // Yellow
        "\033[37m"   // White
    );
    
    // Verify all colors are retrieved in order
    TEST_ASSERT_EQUAL_STRING("\033[91m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[92m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[94m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[93m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[95m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[96m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[31m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[35m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[33m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[37m", lumberjack_get_next_color());
    
    // Should be empty now
    TEST_ASSERT_EQUAL_STRING("", lumberjack_get_next_color());
}

void test_mixed_usage_manual_and_macro(void) {
    // Add some manually
    lumberjack_release_color("\033[90m");  // Dark gray
    lumberjack_release_color("\033[97m");  // Bright white
    
    // Add some via macro
    LUMBERJACK_COLOR_CODES(
        "\033[91m",  // Bright Red
        "\033[92m"   // Bright Green
    );
    
    // Verify order: manual first, then macro
    TEST_ASSERT_EQUAL_STRING("\033[90m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[97m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[91m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("\033[92m", lumberjack_get_next_color());
    TEST_ASSERT_EQUAL_STRING("", lumberjack_get_next_color());
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_empty_queue_returns_empty_string);
    RUN_TEST(test_single_color_add_and_get);
    RUN_TEST(test_single_color_queue_becomes_empty_after_get);
    RUN_TEST(test_multiple_colors_fifo_order);
    RUN_TEST(test_queue_empty_after_removing_all_items);
    RUN_TEST(test_queue_wraparound);
    RUN_TEST(test_queue_full_ignores_additional_items);
    RUN_TEST(test_LUMBERJACK_COLOR_CODES_macro);
    RUN_TEST(test_LUMBERJACK_COLOR_CODES_macro_large_set);
    RUN_TEST(test_mixed_usage_manual_and_macro);
    
    return UNITY_END();
}