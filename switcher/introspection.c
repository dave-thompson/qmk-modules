#ifdef COMMUNITY_MODULE_SWITCHER_ENABLE

    #ifdef SWITCHER_MACOS_APP_SWITCHER
        uint16_t SWITCHER_VIRTUAL_HOLD_KEY = KC_LGUI;
        uint16_t SWITCHER_VIRTUAL_TAP_KEY = KC_TAB;
    #endif //SWITCHER_MACOS_APP_SWITCHER

    #ifdef SWITCHER_ENABLE_SECONDARY_KEYS

        /**
         * switcher_seconday_keys_count(void): Returns the number of secondary keys specified in keymap.c
         * 
         * If you don't want to define your secondary keys in an array, as outlined
         * in switcher.h, then simply define them in the way you wish and override
         * BOTH switcher_secondary_keys_count() and switcher_secondary_keys_get()
         * in your keymap.c file.
         * 
         * You may refer to the raw implementations below in your overriding methods
         * if useful.
         * 
         */
        uint16_t switcher_secondary_keys_count_raw(void) {
            return ARRAY_SIZE(switcher_secondary_keys);
        }

        __attribute__((weak)) uint16_t switcher_secondary_keys_count(void) {
            return switcher_secondary_keys_count_raw();
        }

        /**
         * switcher_secondary_keys_get(uint16_t index): returns the key at index
         * 
         */
        const switcher_key_t* switcher_secondary_keys_get_raw(uint16_t index) {
            if (index >= switcher_secondary_keys_count_raw()) {
                return NULL;
            }
            return &switcher_secondary_keys[index];
        }

        __attribute__((weak)) const switcher_key_t* switcher_secondary_keys_get(uint16_t index) {
            return switcher_secondary_keys_get_raw(index);
        }

    #endif //SWITCHER_ENABLE_SECONDARY_KEYS

#endif //COMMUNITY_MODULE_SWITCHER_ENABLE