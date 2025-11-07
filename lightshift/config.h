#define TAPPING_TERM_PER_KEY

// Enable per-key Permissive Hold so we can disable it for lightshift keys
// while keeping it enabled for other keys
#if defined(PERMISSIVE_HOLD) && !defined(LIGHTSHIFT_USER_PERMISSIVE_HOLD)
    #ifndef PERMISSIVE_HOLD_PER_KEY
        #define PERMISSIVE_HOLD_PER_KEY
    #endif
#endif
