#include <string.h>
#include "quantum.h" // TEMP - DELETE THIS INCLUDE
#include "lumberjack_colors.h"


// State
static const char* color_queue[LUMBERJACK_MAX_COLORS];
static uint8_t queue_front = 0;
static uint8_t queue_rear = 0;
static uint8_t queue_count = 0;



// TEMPORARY - DELETE !!
void lumberjack_print_queue(void) {
    for (int i = 0; i < LUMBERJACK_MAX_COLORS; i++) {
        uprintf("%u: %s\n", i, color_queue[i]);
    }    
}

/**
 * Returns the first item from the top of the queue (and removes that item from the queue)
 * Returns an empty string if the queue is empty
 */
const char* lumberjack_get_next_color(void) {
    if (queue_count == 0) {
        return "";
    }
    
    const char* color = color_queue[queue_front];
    queue_front = (queue_front + 1) % LUMBERJACK_MAX_COLORS;
    queue_count--;
    
    return color;
}


/**
 * Adds the supplied string to the bottom of the queue
 * Does nothing if the queue is full
 */
void lumberjack_release_color(const char* color_code) {
    if (queue_count >= LUMBERJACK_MAX_COLORS) {
        return; // queue full
    }
    
    color_queue[queue_rear] = color_code;
    queue_rear = (queue_rear + 1) % LUMBERJACK_MAX_COLORS;
    queue_count++;
}


void lumberjack_reset_colors(void) {
    queue_front = 0;
    queue_rear = 0;
    queue_count = 0;
}
