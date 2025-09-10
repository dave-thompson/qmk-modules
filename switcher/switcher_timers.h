#pragma once
#include "quantum.h"

/**
 * Time in QMK
 * -----------
 * 
 * QMK keyboards measure the time since boot, in milliseconds. This time is
 * stored as a uint16_t and overflows every 65,536 milliseconds (just over 
 * a minute).
 * 
 *
 * Switcher Timers
 * ---------------
 * 
 * A timer is just a uint16_t variable. You pass this variable to 
 * begin_timer() along with the duration you wish the timer to run for 
 * (in milliseconds), e.g. begin_timer(&my_timer, 1000). Switcher then
 * sets the timer variable to the required deadline (= current time + 
 * specified duration).
 * 
 * You must then regularly poll time_up(&my_timer) to see if the timer has 
 * expired. The first time (and only the first time) you poll 
 * time_up(&my_timer) after the timer has expired, the function will
 * return true. If the timer has not expired, if the timer is not running, 
 * or if the timer was already reported as expired since it was last begun, 
 * then the function will return false.
 * 
 * 
 * Timer Overflow
 * --------------
 * 
 * As timers are uint16_t variables, they are limited to measuring 65,536 
 * milliseconds. Half of this is used to measure time before expiry and half 
 * to measure time after expiry. You must therefore guarantee to poll 
 * time_up() within 32.768 seconds of timer expiry; if not, there is a 50% 
 * chance of an incorrect result.
 */

///////////////////////////////////////////////////////////////////////////////
//
// Boot Timers
//
///////////////////////////////////////////////////////////////////////////////

/* @brief Starts the initial boot timer */
void switcher_start_initial_boot_timer(void);

/** @brief Checks if the initial boot timer has expired
 * 
 *  @return true (only once) if more than SWITCHER_BOOT_DURATION milliseconds 
 *          have passed since the timer was started
 * 
 *  @warning You must poll this function within 32 seconds of timer expiry 
 *           to avoid timer overflow and a potentially incorrect result
 */
bool switcher_initial_boot_timer_ended(void);

#ifdef SWITCHER_MACOS_APP_SWITCHER
    /* @brief Starts the Exposé boot timer */
    void switcher_start_expose_boot_timer(void);

    /* @brief Checks if the Exposé boot timer has expired
     * 
     * @return true (only once) if more than SWITCHER_EXPOSE_BOOT_DURATION 
     *         milliseconds have passed since the timer was started
     *
     * @warning You must poll this function within 32 seconds of timer 
     *          expiry to avoid timer overflow and a potentially incorrect
     *          result
    */
    bool switcher_expose_boot_timer_ended(void);
#endif

/* @brief Checks if we're waiting for either the switcher software or 
 *        Exposé to load
 *
 * @return true if either boot timer is currently running, false otherwise
 *        
 */
 bool switcher_loading(void);

///////////////////////////////////////////////////////////////////////////////
//
// Idle Timer
//
///////////////////////////////////////////////////////////////////////////////

/* @brief Starts or restarts the idle timer */
void switcher_restart_idle_timer(void);

/** @brief Checks if the idle timeout has been reached
 * 
 *  @return true (only once) if the user has been idle for 
 *  SWITCHER_IDLE_TIMEOUT milliseconds
 * 
 *  @warning You must poll this function within 32 seconds of timer expiry 
 *  to avoid timer overflow and a potentially incorrect result
 */
bool switcher_idle_timed_out(void);
