/**
 * @file colors.h
 * @brief FIFO queue for ANSI colour codes
 * 
 * This library provides a simple FIFO queue for storing and cycling through
 * ANSI colour escape codes. Despite the colour-specific naming, it can store
 * any string pointers.
 * 
 * Call the LUMBERJACK_COLOR_CODES macro from an initialisation function to declare your
 * colour palette.
 * 
 * Use lumberjack_get_next_color() to get an unused colour from the palette. Use
 * lumberjack_release_color(color_code) to tell the library when you're no longer using a
 * specific colour.  lumberjack_get_next_color() will always return the _least_recently_used_
 * colour to reduce the chance of the user seeing the same colour used for different
 * purposes at the same time.
 * 
 * @note This library stores pointers to strings, not copies. Ensure the
 *       strings remain valid for the lifetime of their use in the queue.
 * 
 * @author dave-thompson
 */

#pragma once
#include <stdint.h>

// For tests: Ignore PROGMEM-specific code
#ifndef PROGMEM
    #define PROGMEM
#endif
#ifndef memcpy_P
    #define memcpy_P memcpy
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum number of color codes that can be stored in the queue
 * 
 * This can be overridden by defining LUMBERJACK_MAX_COLORS before including this header.
 * 
 */
#ifndef LUMBERJACK_MAX_COLORS
#define LUMBERJACK_MAX_COLORS 10
#endif

// TEMP - DELETE THIS FUNCTION
void lumberjack_print_queue(void);

/**
 * @brief Get the next colour code from the queue
 * 
 * Removes the first item from the front of the queue and returns it to the caller.
 * 
 * @return const char* Pointer to the color code string, or empty string "" if
 *         queue is empty
 * 
 * @note The returned pointer is valid as long as the original string
 *       (supplied in lumberjack_release_color or the LUMBERJACK_COLOR_CODES macro) remains valid
 * 
 * Example usage:
 * @code
 * const char* color = lumberjack_get_next_color();
 * if (strlen(color) > 0) {
 *     printf("%sColored text\033[0m\n", color);
 * }
 * @endcode
 */
const char* lumberjack_get_next_color(void);


/**
 * @brief Add a color code to the queue
 * 
 * Adds the supplied string pointer to the back of the queue.
 * This function implements FIFO (First In, First Out) behavior.
 * 
 * @param color_code Pointer to the color code string to add
 * 
 * @note This function stores the pointer, not a copy of the string.
 *       The string must remain valid for as long as it might be in the queue.
 * @note If the queue is full, this function silently does nothing.
 * 
 * Example usage:
 * @code
 * lumberjack_release_color("\033[91m");  // Add bright red
 * lumberjack_release_color("\033[92m");  // Add bright green
 * @endcode
 */
void lumberjack_release_color(const char* color_code);

/**
 * @brief Reset the queue
 * 
 * Removes all colours from the queue
 */
void lumberjack_reset_colors(void);

/**
 * @brief Macro for initializing the queue with multiple color codes
 * 
 * This macro provides a convenient way to initialize the queue with multiple
 * color codes at once. It must be called from within a function (typically
 * an initialization function such as keyboard_post_init_user()).
 * 
 * @param ... Variable number of string literals (color codes)
 * 
 * @note This macro is equivalent to calling lumberjack_release_color() multiple times
 * @note Must be called from within a function, not at file scope
 * @note Maximum number of arguments depends on your compiler, but typically 127+
 * 
 * Example usage:
 * @code
 * void keyboard_post_init_user(void) {
 *     LUMBERJACK_COLOR_CODES(
 *         "\033[91m",  // Bright Red
 *         "\033[92m",  // Bright Green  
 *         "\033[94m",  // Bright Blue
 *         "\033[93m",  // Bright Yellow
 *         "\033[95m",  // Bright Magenta
 *         "\033[96m",  // Bright Cyan
 *         "\033[31m",  // Red
 *         "\033[35m",  // Magenta
 *         "\033[33m",  // Yellow
 *         "\033[37m"   // White
 *     );
 * }
 * @endcode
 */
#define LUMBERJACK_COLOR_CODES(...) \
    do { \
        static const char* const colors[] PROGMEM = { __VA_ARGS__ }; \
        for (uint8_t i = 0; i < sizeof(colors)/sizeof(colors[0]); i++) { \
            const char* color_ptr; \
            memcpy_P(&color_ptr, &colors[i], sizeof(char*)); \
            lumberjack_release_color(color_ptr); \
        } \
    } while(0)


#ifdef __cplusplus
}
#endif
