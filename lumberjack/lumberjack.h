/**
 * @file lumberjack.h
 * 
 * @author dave-thompson
 */

#pragma once


/**
 * @brief Turn on lumberjack's logging
 * 
 * Logging is on by default.  You do not need to call this unless you disabled
 * logging previously.
 * 
 */
void lumberjack_on(void);


/**
 * @brief Turn off lumberjack's logging
 */
void lumberjack_off(void);
