/**
 * @file lumberjack_utils.h
 * 
 * @brief Lightweight QMK string formatting utilities
 * 
 * This library provides concise alternatives to heavyweight C library
 * functions like snprintf, allowing for reduced firmware size.
 * 
 * Input validation is deliberately minimal; please take care.
 * 
 * @author dave-thompson
 */

#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Calculate the length of a null-terminated string
 * 
 * Lightweight replacement for strlen() that avoids pulling in the entire
 * C standard library.
 * 
 * @param str Pointer to null-terminated string
 * 
 * @return Length of the string in characters (excluding null terminator)
 */
uint8_t lumberjack_str_len(const char* str);


/**
 * @brief Copy string to fixed-size destination buffer with right alignment
 * 
 * Copies the source string to the destination buffer, right-aligning it by
 * padding with spaces on the left. If the source string is longer than the
 * destination buffer, it will instead be truncated from the left.
 * 
 * @param dest Destination buffer (must be different from src)
 * @param dest_size Size of destination buffer including null terminator
 * @param src Source null-terminated string to copy
 * 
 * @warning dest and src must be different
 */
void lumberjack_right_align_string(char* dest, uint8_t dest_size,
                                   const char* src);


/**
 * @brief Prepend a character to the beginning of a string
 * 
 * Shifts the existing string one position to the right and inserts the
 * specified character at the beginning. The string is modified in-place.
 * 
 * @param str Null-terminated string to modify
 * @param ch Character to prepend to the string
 * 
 * @warning The string buffer must have at least one extra byte of space
 *          available beyond the current string length to accommodate the
 *          new character
 */
void lumberjack_prepend_char(char* str, char ch);


/**
 * @brief Convert QMK keycode to hexadecimal string with 0x prefix
 * 
 * Converts a 16-bit unsigned integer to a hexadecimal string in the format
 * "0x####" where #### represents 4 uppercase hexadecimal digits.
 * 
 * @param dest Destination buffer (must be at least 7 bytes: "0x1234\0")
 * @param value 16-bit unsigned integer to convert (such as a QMK keycode)
 */
void lumberjack_keycode_to_hex_string(char* dest, uint16_t value);


/**
 * @brief Convert unsigned 16-bit integer to decimal string
 * 
 * Converts an unsigned 16-bit integer to its decimal string representation.
 * 
 * @param dest Destination buffer (must be at least 6 bytes for max value
 *             65535: 5 chars + 1 null terminator)
 * @param value Unsigned 16-bit integer to convert (0-65535)
 */
void lumberjack_uint_to_string(char* dest, uint16_t value);


#ifdef __cplusplus
}
#endif
