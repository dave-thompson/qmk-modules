#include <string.h>
#include <stdio.h>
#include "lumberjack_color_queue.h"


// State
static const char* color_queue[LUMBERJACK_MAX_COLORS];
static uint8_t queue_front = 0;
static uint8_t queue_rear = 0;
static uint8_t queue_count = 0;


// Adds the supplied string to the bottom of the queue
// Does nothing if the queue is full
void lumberjack_add_color_to_queue(const char* color) {
    if (!color) return;
    if (queue_count >= LUMBERJACK_MAX_COLORS) {
        return; // queue full
    }
    
    color_queue[queue_rear] = color;
    queue_rear = (queue_rear + 1) % LUMBERJACK_MAX_COLORS;
    queue_count++;
}


// Returns the item at the top of the queue (and removes it from the queue)
// Returns an empty string if the queue is empty
const char* lumberjack_next_color(void) {
    if (queue_count == 0) {
        return "";
    }
    
    const char* color = color_queue[queue_front];
    if (!color) {
        color = "";  // fallback to empty string
    }
    queue_front = (queue_front + 1) % LUMBERJACK_MAX_COLORS;
    queue_count--;
    return color;
}


// Resets the colour queue, for use in unit tests
void lumberjack_reset_colors(void) {
    queue_front = 0;
    queue_rear = 0;
    queue_count = 0;
}
