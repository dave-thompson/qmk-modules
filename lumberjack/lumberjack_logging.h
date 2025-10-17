/**
 * @file lumberjack_logging.h
 * 
 * @author dave-thompson
 */

#pragma once

#include "lumberjack_tracking.h"

/**
 * @brief Log a physical key movement (DOWN or UP) to the console
 * 
 * @param keypress_data tracking data for the keypress to be logged
 * @param keycode keycode for the keypress to be logged
 * @param delta milliseconds since the preceeding key event
 * @param pressed true for DOWN, false for UP
 * 
 */
void lumberjack_log_input(const keypress_t* keypress_data,
                          uint16_t keycode, uint16_t delta,
                          bool pressed);


/**
 * @brief Dump key event to the console (from process_user / post_process_user)
 * 
 * @param keycode keycode for the keypress to be logged
 * @param record trecord for the keypress to be logged
 * 
 */
void lumberjack_log_interpreted_event(const char *prefix, uint16_t keycode,
                                      keyrecord_t *record);
