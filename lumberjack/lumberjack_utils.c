#include "lumberjack_utils.h"

///////////////////////////////////////////////////////////////////////////////
//
// Properties
//
///////////////////////////////////////////////////////////////////////////////

// Get string length
uint8_t lumberjack_str_len(const char* str, uint8_t max_len) {
    uint8_t len = 0;
    while (str[len] != '\0' && len < max_len) len++;
    return len;
}


///////////////////////////////////////////////////////////////////////////////
//
// String Manipulation
//
///////////////////////////////////////////////////////////////////////////////

// Copy src to dest & align to right
void lumberjack_right_align_string(char* dest, uint8_t dest_size,
                                   const char* src) {
    uint8_t src_len = lumberjack_str_len(src, dest_size);
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


///////////////////////////////////////////////////////////////////////////////
//
// Numeric to String Conversion
//
///////////////////////////////////////////////////////////////////////////////

// Convert keycode to hex string, e.g. 0x320B
void lumberjack_keycode_to_hex_string(char* dest, uint16_t value) {
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
void lumberjack_uint_to_string(char* dest, uint16_t value) {
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
