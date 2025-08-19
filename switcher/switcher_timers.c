#include "switcher_timers.h"
#include "switcher_config.h"

/**
 * Time in QMK
 * -----------
 * 
 * QMK keyboards measure the time since boot, in milliseconds.  This time is
 * stored as a uint16_t and overflows every 65,536 milliseconds (just over a minute).
 * 
 *
 * Switcher Timers
 * ---------------
 * 
 * A timer is just a uint16_t variable.  You pass this variable to begin_timer() along with the duration
 * you wish the timer to run for (in milliseconds), e.g. begin_timer(&my_timer, 1000). Switcher then
 * sets the timer variable to the required deadline (= current time + specified duration).
 * 
 * You must then regularly poll time_up(&my_timer) to see if the timer has expired.  The first time
 * (and only the first time) you poll time_up(&my_timer) after the timer has expired, the function will
 * return true.  If the timer has not expired, if the timer is not running, or if the timer was already
 * reported as expired since it was last begun, then the function will return false.
 * 
 * 
 * Timer Overflow
 * --------------
 * 
 * As timers are uint16_t variables, they are limited to measuring 65,536 milliseconds.  Half of this is used to
 * measure time before expiry and half to measure time after expiry.  You must therefore guarantee to poll time_up()
 * within 32.768 seconds of timer expiry; if not, there is a 50% chance of an incorrect result.
 * 
 */


///////////////////////////////////////////////////////////////////////////////
//
// Timer Utilities
//
///////////////////////////////////////////////////////////////////////////////

/// @brief Starts the specified timer with the requested duration
/// @param timer Pointer to the timer variable to start
/// @param duration Duration in milliseconds (0 disables the timer)
/// @warning duration must be < 32,768ms to avoid timer overflow
static void begin_timer(uint16_t *timer, uint16_t duration) {
    if (duration != 0) // 0 disables timed feature (only for idle timeout; caching never has a duration of < 1)
    {
        // The bitwise OR here, "| 1", increments any even number by 1 to make it odd. This addresses the 
        // corner case where the computed value is 0 (which otherwise would happen roughly 1 in 65k times:
        // 16-bit timers loop every 2^16 = 65536ms), falsely signalling that we're not currently booting.
        // The resulting 1ms inaccuracy is unimportant.
        *timer = (timer_read() + duration) | 1;
    }
}

/// @brief Returns true exactly once per timer, the first time it's called after the specified timer has expired
/// @param timer Pointer to the timer variable to check
/// @return true if timer just expired, false otherwise
/// @warning You must poll this function < 32.7 seconds after timer expiry to prevent timer overflow
static bool timer_finished(uint16_t *timer) {
    // If the specified timer is running (i.e. not 0) and expired
    if (*timer && timer_expired(timer_read(), *timer)) {
        *timer = 0;
        return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////////////////
//
// Boot Timers
//
///////////////////////////////////////////////////////////////////////////////

static uint16_t initial_boot_timer = 0; // Estimated switcher _software_ boot completion time; 0 if and only if app switcher not currently booting

#ifdef SWITCHER_MACOS_APP_SWITCHER
    static uint16_t expose_boot_timer = 0; // Estimated MacOS Expose boot completion time; 0 if and only if window mode not currently booting
    bool switcher_loading(void) { return initial_boot_timer || expose_boot_timer; }
    void switcher_start_expose_boot_timer(void) {
        begin_timer(&expose_boot_timer, SWITCHER_EXPOSE_BOOT_DURATION);
    }
    bool switcher_expose_boot_timer_ended(void) {
        return timer_finished(&expose_boot_timer);
    }
#else
    bool switcher_loading(void) { return initial_boot_timer; }
#endif

// Starts the initial boot timer
void switcher_start_initial_boot_timer(void) {
    begin_timer(&initial_boot_timer, SWITCHER_BOOT_DURATION);
}

// Returns true (only once) if more than SWITCHER_BOOT_DURATION milliseconds have passed since the timer was started
bool switcher_initial_boot_timer_ended(void) {
    return timer_finished(&initial_boot_timer);
}


///////////////////////////////////////////////////////////////////////////////
//
// Idle Timer
//
///////////////////////////////////////////////////////////////////////////////

#ifdef SWITCHER_IDLE_TIMEOUT

    static uint16_t idle_timer = 0;

    // Starts (or restarts) the idle timer
    void switcher_restart_idle_timer(void) { begin_timer(&idle_timer, SWITCHER_IDLE_TIMEOUT); }

    // Returns true (only once) if the user has been idle for SWITCHER_IDLE_TIMEOUT milliseconds
    bool switcher_idle_timed_out(void) { return (timer_finished(&idle_timer)); }

#else // if not defined SWITCHER_IDLE_TIMEOUT
    void switcher_restart_idle_timer(void) {}
    bool switcher_idle_timed_out(void) {return false;}
#endif // SWITCHER_IDLE_TIMEOUT
