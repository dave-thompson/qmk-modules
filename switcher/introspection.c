#if defined(COMMUNITY_MODULE_SWITCHER_ENABLE) && defined(SWITCHER_ENABLE_SECONDARY_KEYS)


uint16_t switcher_secondary_keys_count_raw(void) {
    return ARRAY_SIZE(switcher_secondary_keys);
}

/**
 * @brief Returns the number of secondary keys specified in keymap.c using
 *        the SWITCHER_SECONDARY_KEYS macro
 * 
 * If your secondary keys are defined in a different way, then override
 * switcher_secondary_keys_count() and switcher_secondary_keys_get().
 * 
 * @return number of secondary keys in the user's keymap
 */
__attribute__((weak)) uint16_t switcher_secondary_keys_count(void) {
    return switcher_secondary_keys_count_raw();
}


const switcher_key_t* switcher_secondary_keys_get_raw(uint16_t index) {
    if (index >= switcher_secondary_keys_count_raw()) {
        return NULL;
    }
    return &switcher_secondary_keys[index];
}

/**
 * @brief Returns the requested secondary key as specified using the
 *        SWITCHER_SECONDARY_KEYS macro in the user's keymap
 * 
 * If your secondary keys are defined in a different way, then override
 * switcher_secondary_keys_count() and switcher_secondary_keys_get().
 * 
 * @param index The index of the requested secondary key
 * 
 * @return A secondary key pair, including both (1) the key to be pressed
 *         by the user and (2) the keycode to be sent by Switcher to the
 *         switching software.
 */
__attribute__((weak)) const switcher_key_t* switcher_secondary_keys_get(uint16_t index) {
    return switcher_secondary_keys_get_raw(index);
}


#endif //COMMUNITY_MODULE_SWITCHER_ENABLE && SWITCHER_ENABLE_SECONDARY_KEYS