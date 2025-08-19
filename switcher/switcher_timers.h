
#pragma once

#include "quantum.h"

///////////////////////////////////////////////////////////////////////////////
//
// Boot Timers
//
///////////////////////////////////////////////////////////////////////////////

/// @brief Starts the initial boot timer
void switcher_start_initial_boot_timer(void);

/// @brief Checks if the initial boot timer has expired
/// @return true (only once) if more than SWITCHER_BOOT_DURATION milliseconds have passed since the timer was started
/// @warning You must poll this function within 32 seconds of timer expiry to avoid timer overflow and a potentially incorrect result
bool switcher_initial_boot_timer_ended(void);

#ifdef SWITCHER_MACOS_APP_SWITCHER
    /// @brief Starts the Exposé boot timer
    void switcher_start_expose_boot_timer(void);

    /// @brief Checks if the Exposé boot timer has expired
    /// @return true (only once) if more than SWITCHER_EXPOSE_BOOT_DURATION milliseconds have passed since the timer was started
    /// @warning You must poll this function within 32 seconds of timer expiry to avoid timer overflow and a potentially incorrect result
    bool switcher_expose_boot_timer_ended(void);
#endif

/// @brief Checks if we're waiting for either the switcher software or Exposé to load
/// @return true if either boot timer is currently running, false otherwise
bool switcher_loading(void);


///////////////////////////////////////////////////////////////////////////////
//
// Idle Timer
//
///////////////////////////////////////////////////////////////////////////////

/// @brief Starts or restarts the idle timer
void switcher_restart_idle_timer(void);

/// @brief Checks if the idle timeout has been reached
/// @return true (only once) if the user has been idle for SWITCHER_IDLE_TIMEOUT milliseconds
/// @warning You must poll this function within 32 seconds of timer expiry to avoid timer overflow and a potentially incorrect result
bool switcher_idle_timed_out(void);
