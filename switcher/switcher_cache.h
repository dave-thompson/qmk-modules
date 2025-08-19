
/**
* MacOS only caches 'Tab' in app switching (& 'backtick' in Exposé).
* All other keycodes must be cached at the keyboard level.
*/

#pragma once

#include "quantum.h"

///////////////////////////////////////////////////////////////////////////////
//
// Secondary Key Cache
//     - Caches virtual keycodes only
//
///////////////////////////////////////////////////////////////////////////////

/// Setter: Caches a secondary keycode
void  switcher_cache_secondary_keycode(uint16_t keycode);

/// Getter: Returns the cached secondary keycode at the specifed index
uint16_t switcher_secondary_cache_get_item_at_index(uint8_t index);

/// How many secondary keycodes are currently cached?
uint8_t switcher_secondary_cache_count(void);

/// @brief Resets cache index without flushing cached data
/// 
/// Allows for all keys in the cache to be processed (and re-cached if necessary) while using no additional memory.
/// 
/// @return The number of keys in the cache.
/// 
/// @warning Any cached keys must be processed _immediately_ after calling this function or they may be overwritten.
/// 
/// Example implementation:
/// @code
/// // Reset the cache index and store the number of cached keys before the reset
/// uint8_t num_keys = switcher_reset_secondary_cache_index();
/// // Immediately process the cached keys before they are overwritten
/// for (uint8_t i = 0; i < num_keys; i++) {
///     // act on the key at index i
///     // recache it if necessary using switcher_cache_secondary_keycode(uint16_t keycode);
/// }
/// @endcode
uint8_t switcher_reset_secondary_cache_index(void);


///////////////////////////////////////////////////////////////////////////////
//
// Ending Key Cache
//
///////////////////////////////////////////////////////////////////////////////

/// Setter: Caches an ending key
void switcher_cache_ending_key(uint16_t keycode, keyrecord_t *record);

/// Getter: Returns the record for the cached ending key
keyrecord_t* switcher_cached_ending_record(void);

//// Getter: Returns the keycode for the cached ending key
uint16_t switcher_cached_ending_keycode(void);

/// Is there an ending key cached?
bool switcher_is_ending_key_cached(void);


///////////////////////////////////////////////////////////////////////////////
//
// Cache Clearing
//
///////////////////////////////////////////////////////////////////////////////

/// Clears all caches (secondary key & ending key)
void switcher_reset_cache(void);
