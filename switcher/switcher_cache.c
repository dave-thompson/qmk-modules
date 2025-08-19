#include "switcher_cache.h"
#include "switcher_config.h"

///////////////////////////////////////////////////////////////////////////////
//
// Secondary Key Cache
//     - Caches virtual keycodes only
//
///////////////////////////////////////////////////////////////////////////////

static uint16_t secondary_key_cache[SWITCHER_SECONDARY_KEY_CACHE_SIZE];
static uint8_t secondary_key_cache_count = 0;

// Setter: Caches a secondary keycode
void switcher_cache_secondary_keycode(uint16_t keycode) {
    if (secondary_key_cache_count < SWITCHER_SECONDARY_KEY_CACHE_SIZE) {
        secondary_key_cache[secondary_key_cache_count] = keycode;
        secondary_key_cache_count++;
    }
}

// Getter: Returns the cached secondary keycode at the specifed index
uint16_t switcher_secondary_cache_get_item_at_index(uint8_t index) {
    return secondary_key_cache[index];
}

// How many secondary keycodes are currently cached?
uint8_t switcher_secondary_cache_count(void) {
    return secondary_key_cache_count;
}

// Resets the secondary cache index without flushing the cached data
// Any cached data must be processed immediately after calling this function or it may be overwritten.
// Allows for a full cache of keycodes to be processed and re-cached if necessary, without doubling the size of the cache.
uint8_t switcher_reset_secondary_cache_index(void) {
    uint8_t prior_cache_count = secondary_key_cache_count;
    secondary_key_cache_count = 0;
    return prior_cache_count;
}


///////////////////////////////////////////////////////////////////////////////
//
// Ending Key Cache
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    bool has_key;         // is there a cached ending key?
    keyrecord_t record;   // cached key's record
    uint16_t keycode;     // cached key's keycode
} ending_cache_t;

static ending_cache_t ending_cache = {0};


// Setter: Caches an ending key
void switcher_cache_ending_key(uint16_t keycode, keyrecord_t *record) {
    ending_cache.record = *record;
    ending_cache.keycode = keycode;
    ending_cache.has_key = true;
}

// Getter: Returns the record for the cached ending key
keyrecord_t* switcher_cached_ending_record(void) {
    return &ending_cache.record;
}

// Getter: Returns the keycode for the cached ending key
uint16_t switcher_cached_ending_keycode(void) {
    return ending_cache.keycode;
}

// Is there an ending key cached?
bool switcher_is_ending_key_cached(void) {
    return ending_cache.has_key;
}


///////////////////////////////////////////////////////////////////////////////
//
// Cache Clearing
//
///////////////////////////////////////////////////////////////////////////////

// Clears all caches (secondary key & ending key)
void switcher_reset_cache(void) {
    ending_cache = (ending_cache_t){0};
    #ifdef SWITCHER_ENABLE_SECONDARY_KEYS
        secondary_key_cache_count = 0;
    #endif
}
