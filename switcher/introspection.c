#ifdef COMMUNITY_MODULE_SWITCHER_ENABLE

uint16_t switcher_secondary_keys_count_raw(void) {
  return ARRAY_SIZE(switcher_secondary_keys);
}

__attribute__((weak)) uint16_t switcher_secondary_keys_count(void) {
  return switcher_secondary_keys_count_raw();
}

const switcher_key_t* switcher_secondary_keys_get_raw(uint16_t index) {
  if (index >= switcher_secondary_keys_count_raw()) {
    return NULL;
  }
  return &switcher_secondary_keys[index];
}

__attribute__((weak)) const switcher_key_t* switcher_secondary_keys_get(
    uint16_t index) {
  return switcher_secondary_keys_get_raw(index);
}

#endif // COMMUNITY_MODULE_SWITCHER_ENABLE