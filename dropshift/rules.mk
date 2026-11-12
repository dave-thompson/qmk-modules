# notify other modules that dropshift is enabled
OPT_DEFS += -DDROPSHIFT_ENABLE

# add source files
SRC += dropshift_state.c

# auto-install caps word (unless user disables)
CAPS_WORD_ENABLE ?= yes
