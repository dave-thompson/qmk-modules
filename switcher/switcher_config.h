
#pragma once

///////////////////////////////////////////////////////////////////////////////
//
// Defaults
//
///////////////////////////////////////////////////////////////////////////////

// Mac Hold & Tap keys
#ifdef SWITCHER_MACOS_APP_SWITCHER
    #ifndef SWITCHER_VIRTUAL_HOLD_KEY
        #define SWITCHER_VIRTUAL_HOLD_KEY KC_LGUI
    #endif
    #ifndef SWITCHER_VIRTUAL_TAP_KEY
        #define SWITCHER_VIRTUAL_TAP_KEY KC_TAB
    #endif
#endif

// Cache Size
#ifndef SWITCHER_SECONDARY_KEY_CACHE_SIZE
    #define SWITCHER_SECONDARY_KEY_CACHE_SIZE 8
#endif

// Boot Timers

// Maximum time in ms for the switcher _software_ to boot up after the 
// SWITCHER keystroke is sent; during this time window, any keystrokes will 
// be cached and then sent once boot is expected to have completed
#ifndef SWITCHER_BOOT_DURATION
    #define SWITCHER_BOOT_DURATION 180 // 180ms typical for MacOS app switcher
#endif

// Maximum time in ms for the stock MacOS app switcher to load the window 
// manager; during this time window, any keystrokes will be cached and then 
// sent once loading is expected to have completed
#if defined(SWITCHER_MACOS_APP_SWITCHER) && \
    !defined(SWITCHER_EXPOSE_BOOT_DURATION)
    #define SWITCHER_EXPOSE_BOOT_DURATION 400 // Exposé takes longer to boot
#endif

// Custom Macros: Time to pause before opening a window from Expose
// Expose freezes for around 150-200ms after navigating to a new window
#define SWITCHER_EXPOSE_EXIT_DELAY 250

///////////////////////////////////////////////////////////////////////////////
//
// Compile-time Checks
//
///////////////////////////////////////////////////////////////////////////////

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

// Check Hold & Tap keys defined
#ifndef SWITCHER_VIRTUAL_HOLD_KEY
  #error "Define SWITCHER_VIRTUAL_HOLD_KEY (or SWITCHER_MACOS_APP_SWITCHER)"
#endif
#ifndef SWITCHER_VIRTUAL_TAP_KEY
  #error "Define SWITCHER_VIRTUAL_TAP_KEY (or SWITCHER_MACOS_APP_SWITCHER)"
#endif

// Constrain boot durations:
//   - Must be at least 1 millisecond, so that boot timers always run and 
//     post-boot processing is always carried out.
//   - Must not exceed 30 seconds, so as not to overflow the 16-bit timer 
//     (32,768ms).
#if SWITCHER_BOOT_DURATION < 1 || SWITCHER_BOOT_DURATION > 30000
    #error "SWITCHER_BOOT_DURATION must be between 1 and 30000 ms"
#endif
#if defined(SWITCHER_EXPOSE_BOOT_DURATION) && \
        (SWITCHER_EXPOSE_BOOT_DURATION < 1 || \
         SWITCHER_EXPOSE_BOOT_DURATION > 30000)
    #error "SWITCHER_EXPOSE_BOOT_DURATION must be between 1 and 30000 ms"
#endif


///////////////////////////////////////////////////////////////////////////////
//
// C-functions for cleaner config access
//
///////////////////////////////////////////////////////////////////////////////

inline bool using_macos_switcher(void) {
    #ifdef SWITCHER_MACOS_APP_SWITCHER
        return true;
    #else
        return false;
    #endif
}

inline bool switcher_secondary_keys_enabled(void) {
    #ifdef SWITCHER_ENABLE_SECONDARY_KEYS
        return true;
    #else
        return false;
    #endif
}
