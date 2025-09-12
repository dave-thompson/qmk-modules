/**
 * @file lumberjack.h
 * 
 * @author dave-thompson
 */

#pragma once


/**
 * @brief Turn on lumberjack's logging
 * 
 * Logging is on by default.  You do not need to call this unless you called
 * disable_lumberjack() previously.
 * 
 */
void enable_lumberjack(void);


/**
 * @brief Turn off lumberjack's logging
 */
void disable_lumberjack(void);