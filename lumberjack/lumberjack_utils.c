#include "lumberjack_utils.h"

///////////////////////////////////////////////////////////////////////////////
//
// Properties
//
///////////////////////////////////////////////////////////////////////////////

// Get string length
// Returns num of chars before '\0' terminator (OR max_len, if smaller)
uint8_t lumberjack_str_len(const char* str, uint8_t max_len) {
    if (!str) return 0;
    uint8_t len = 0;
    while (str[len] != '\0' && len < max_len) len++;
    return len;
}


///////////////////////////////////////////////////////////////////////////////
//
// String Alignment
//
///////////////////////////////////////////////////////////////////////////////

// Copy src to dest & align to right
void lumberjack_right_align_string(char* dest, uint8_t dest_size,
                                   const char* src) {
    if (!dest || !src || dest_size == 0) return;

    // Set source length to smaller of src or dest, to prevent buffer overrun
    uint8_t src_len = lumberjack_str_len(src, dest_size);

    uint8_t i;
    // If source is longer than destination, truncate end
    if (src_len >= dest_size) {
        for (i = 0; i < dest_size - 1; i++) {
            dest[i] = src[i];
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
void lumberjack_keycode_to_hex_string(char* dest, uint8_t dest_size,
                                      uint16_t value) {
    if (!dest || dest_size < 7) return;  // Need at least "0x1234\0"

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
void lumberjack_uint_to_string(char* dest, uint8_t dest_size,
                               uint16_t value) {
    if (!dest || dest_size == 0) return;
    
    // Handle zero case
    if (value == 0) {
        if (dest_size >= 2) {  // Need space for "0\0"
            dest[0] = '0';
            dest[1] = '\0';
        }
        return;
    }
    
    // Write digits to temp in reverse order
    char temp[6];  //  Max 16-bit uint is 65535 (5 chars) + null
    uint8_t len = 0;    
    while (value > 0 && len < 5) {
        temp[len++] = '0' + (value % 10);
        value /= 10;
    }
    
    // If supplied buffer too small, reduce len to truncate value
    if (dest_size < len + 1) {
        len = dest_size - 1;
    }
    
    // Reverse digits from temp into dest buffer
    for (uint8_t i = 0; i < len; i++) {
        dest[i] = temp[len - 1 - i];
    }
    dest[len] = '\0';
}
