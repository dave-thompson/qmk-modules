#include "switcher_cache.h"
#include "switcher_config.h"

///////////////////////////////////////////////////////////////////////////////
//
// Secondary Key Cache
//
//     - Caches virtual keycodes only.
//
///////////////////////////////////////////////////////////////////////////////

static uint16_t secondary_key_cache[SWITCHER_SECONDARY_KEY_CACHE_SIZE];
static uint8_t secondary_key_cache_count = 0;

void switcher_cache_secondary_keycode(uint16_t keycode) {
    // silently drop the key if the cache is full
    if (secondary_key_cache_count < SWITCHER_SECONDARY_KEY_CACHE_SIZE) {
        secondary_key_cache[secondary_key_cache_count] = keycode;
        secondary_key_cache_count++;
    }
}

uint16_t switcher_secondary_cache_get_item_at_index(uint8_t index) {
    return secondary_key_cache[index];
}

uint8_t switcher_secondary_cache_count(void) {
    return secondary_key_cache_count;
}

// Allows immediate reprocessing of cached keys without additional memory
// allocation
uint8_t switcher_reset_secondary_cache_index(void) {
    uint8_t prior_cache_count = secondary_key_cache_count;
    // reset the cache count, but don't flush the data
    secondary_key_cache_count = 0;
    // return the cache count as it was prior to the reset, so the user
    // may process the data even after we've reset the count
    return prior_cache_count;
}


///////////////////////////////////////////////////////////////////////////////
//
// Ending Key Cache
//
//     - Unlike secondary keys, cached ending keys need both the keycode and
//       the full corresponding record.  At most one ending key is cached
//       during each Switcher invocation.
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    bool has_key;         // is there a cached ending key?
    keyrecord_t record;   // cached key's record
    uint16_t keycode;     // cached key's keycode
} ending_cache_t;

static ending_cache_t ending_cache = {0};


void switcher_cache_ending_key(uint16_t keycode, keyrecord_t *record) {
    ending_cache.record = *record;
    ending_cache.keycode = keycode;
    ending_cache.has_key = true;
}

keyrecord_t* switcher_cached_ending_record(void) {
    return &ending_cache.record;
}

uint16_t switcher_cached_ending_keycode(void) {
    return ending_cache.keycode;
}

bool switcher_is_ending_key_cached(void) {
    return ending_cache.has_key;
}


///////////////////////////////////////////////////////////////////////////////
//
// Cache Clearing
//
///////////////////////////////////////////////////////////////////////////////

void switcher_reset_cache(void) {
    ending_cache = (ending_cache_t){0};
    #ifdef SWITCHER_ENABLE_SECONDARY_KEYS
        secondary_key_cache_count = 0;
    #endif
}
