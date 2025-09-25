#include "unity/unity.h"
#include "../lumberjack_utils.h"
#include <string.h>

void setUp(void) {}

void tearDown(void) {}

void test_str_len_should_return_correct_length(void) {
    TEST_ASSERT_EQUAL(0, lumberjack_str_len("", 50));
    TEST_ASSERT_EQUAL(10, lumberjack_str_len("lumberjack", 50));
    TEST_ASSERT_EQUAL(1, lumberjack_str_len("1", 50));
}

void test_right_align_string_should_pad_correctly(void) {
    char buffer[14+1];

    lumberjack_right_align_string(buffer, 14+1, "lumberjack");
    TEST_ASSERT_EQUAL_STRING("    lumberjack", buffer);
}

void test_right_align_string_should_truncate_long_strings(void) {
    char buffer[7+1];

    lumberjack_right_align_string(buffer, 7+1, "lumberjack");
    TEST_ASSERT_EQUAL_STRING("lumberj", buffer);
}

void test_keycode_to_hex_string_should_format_correctly(void) {
    char buffer[6+1];
    
    lumberjack_keycode_to_hex_string(buffer, 6+1, 0x1234);
    TEST_ASSERT_EQUAL_STRING("0x1234", buffer);
    
    lumberjack_keycode_to_hex_string(buffer, 6+1, 0xABCD);
    TEST_ASSERT_EQUAL_STRING("0xABCD", buffer);
}

void test_uint_to_string_should_convert_correctly(void) {
    char buffer[5+1];
    
    lumberjack_uint_to_string(buffer, 5+1, 0);
    TEST_ASSERT_EQUAL_STRING("0", buffer);
    
    lumberjack_uint_to_string(buffer, 5+1, 1038);
    TEST_ASSERT_EQUAL_STRING("1038", buffer);
    
    lumberjack_uint_to_string(buffer, 5+1, 65535);
    TEST_ASSERT_EQUAL_STRING("65535", buffer);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_str_len_should_return_correct_length);
    RUN_TEST(test_right_align_string_should_pad_correctly);
    RUN_TEST(test_right_align_string_should_truncate_long_strings);
    RUN_TEST(test_keycode_to_hex_string_should_format_correctly);
    RUN_TEST(test_uint_to_string_should_convert_correctly);
    
    return UNITY_END();
}