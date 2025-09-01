/**
 * @file lumberjack_colors.h
 * @brief FIFO queue for ANSI colour codes
 * 
 * This library provides a simple FIFO queue for storing and cycling through
 * ANSI colour escape codes. Despite the colour-specific naming, it can store
 * any string pointers.
 * 
 * Call lumberjack_add_color_to_queue from an initialisation function as many times as 
 * necessary to declare your colour palette.
 * 
 * Use lumberjack_next_color() to get an unused colour from the palette. Use
 * lumberjack_add_color_to_queue(color_code) to tell the library when you're no longer using a
 * specific colour.  lumberjack_next_color() will always return the _least_recently_used_
 * colour to reduce the chance of the user seeing the same colour used for different
 * purposes at the same time.
 * 
 * @note This library stores pointers to strings, not copies. Ensure the
 *       strings remain valid for the lifetime of their use in the queue.
 * 
 * @note Despite the British English comments, all code uses American spellings for better
 *       portability.
 * 
 * @author dave-thompson
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Maximum number of colour codes that can be stored in the queue
 * 
 */
#ifndef LUMBERJACK_MAX_COLORS
#define LUMBERJACK_MAX_COLORS 5
#endif


/**
 * @brief Add a colour code to the FIFO queue
 * 
 * Adds the supplied string pointer to the back of the queue.
 * 
 * @param color Pointer to the colour code string to add
 * 
 * @note This function stores the pointer, not a copy of the string.
 *       The string must remain valid for as long as it might be in the queue.
 * @note If the queue is full, this function silently does nothing.
 * 
 * Example usage:
 * @code
 * lumberjack_add_color_to_queue("\033[91m");  // Add bright red
 * lumberjack_add_color_to_queue("\033[92m");  // Add bright green
 * @endcode
 */
void lumberjack_add_color_to_queue(const char* color);


/**
 * @brief Get the next colour code from the queue
 * 
 * Removes the first item from the front of the queue and returns it to the caller.
 * 
 * @return const char* Pointer to the colour code string, or empty string "" if
 *         queue is empty
 * 
 * @note The returned pointer is valid as long as the original string
 *       (supplied in lumberjack_add_color_to_queue) remains valid
 * 
 * Example usage:
 * @code
 * const char* color = lumberjack_next_color();
 * if (strlen(color) > 0) {
 *     printf("%sColored text\033[0m\n", color);
 * }
 * @endcode
 */
const char* lumberjack_next_color(void);


/**
 * @brief Reset the queue
 * 
 * Removes all colours from the queue
 */
void lumberjack_reset_colors(void);


/**
 * @brief Convenience method for access to LUMBERJACK_COLOR config parameter
 */
inline bool lumberjack_color(void) {
    #ifdef LUMBERJACK_COLOR
        return true;
    #else
        return false;
    #endif
}


#ifdef __cplusplus
}
#endif
