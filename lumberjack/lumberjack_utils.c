#include "lumberjack_utils.h"

///////////////////////////////////////////////////////////////////////////////
//
// Properties
//
///////////////////////////////////////////////////////////////////////////////

// Get string length
uint8_t str_len(const char* str) {
    uint8_t len = 0;
    while (str[len] != '\0') len++;
    return len;
}


///////////////////////////////////////////////////////////////////////////////
//
// String Manipulation
//
///////////////////////////////////////////////////////////////////////////////

// Copy src to dest & align to right
void right_align_string(char* dest, uint8_t dest_size, const char* src) {
    uint8_t src_len = str_len(src);
    uint8_t i;
    
    // If source is longer than destination, truncate from left
    if (src_len >= dest_size) {
        for (i = 0; i < dest_size - 1; i++) {
            dest[i] = src[src_len - dest_size + 1 + i];
        }
    } else {
        // Add spaces for left padding
        for (i = 0; i < dest_size - 1 - src_len; i++) {
            dest[i] = ' ';
        }
        // Copy source string to remaining spaces
        for (i = 0; i < src_len; i++) {
            dest[dest_size - 1 - src_len + i] = src[i];
        }
    }
    dest[dest_size - 1] = '\0';
}

// Prepend a character to a string
// @note Uses a lightweight memmove implementation to safely handle
//       overlapping memory regions
void prepend_char(char* str, char ch) {
    uint8_t len = str_len(str);
    
    // Shift everything right by 1, starting from the end (including null terminator)
    for (uint8_t i = len + 1; i > 0; i--) {
        str[i] = str[i - 1];
    }
    
    // Insert new character at beginning
    str[0] = ch;
}


///////////////////////////////////////////////////////////////////////////////
//
// Numeric to String Conversion
//
///////////////////////////////////////////////////////////////////////////////

// Convert keycode to hex string, e.g. 0x320B
void keycode_to_hex_string(char* dest, uint16_t value) {
    const char hex_chars[] = "0123456789ABCDEF";
    uint8_t i;
        
    // Add "0x" prefix
    dest[0] = '0';
    dest[1] = 'x';
    
    // Convert to hex (4 digits, so 16 bits max)
    for (i = 0; i < 4; i++) {
        dest[2 + i] = hex_chars[(value >> (12 - i * 4)) & 0xF];
    }
    dest[6] = '\0';
}

// Convert unsigned int to string
void uint_to_string(char* dest, uint16_t value) {
    uint8_t len = 0;
    uint8_t i;
    
    // Handle zero case
    if (value == 0) {
        dest[0] = '0';
        dest[1] = '\0';
        return;
    }
    
    // Convert digits in reverse order
    char temp[6]; // Enough for 16-bit unsigned int
    while (value > 0) {
        temp[len++] = '0' + (value % 10);
        value /= 10;
    }
    
    // Copy digits in correct order (reverse from temp)
    for (i = 0; i < len; i++) {
        dest[i] = temp[len - 1 - i];
    }
    dest[len] = '\0';
}
