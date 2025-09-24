/**
 * @file lumberjack_logging.h
 * 
 * @author dave-thompson
 */

#pragma once

#include "lumberjack_tracking.h"

/**
 * @brief Log a key event (DOWN or UP) to the console
 * 
 * @param keypress_data tracking data for the keypress to be logged
 * @param keycode keycode for the keypress to be logged
 * @param delta milliseconds since the preceeding key event
 * @param pressed true for DOWN, false for UP
 * 
 */
void lumberjack_log_event(const keypress_t* keypress_data,
                                 uint16_t keycode, uint16_t delta,
                                 bool pressed);
