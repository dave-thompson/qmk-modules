# notify other modules that lightshift is enabled
OPT_DEFS += -DLIGHTSHIFT_ENABLE

# add source files
SRC += lightshift_state.c
SRC += lightshift_tracking.c
SRC += lightshift_drop.c

# dropshift enabled unless user disables
DROPSHIFT_ENABLE ?= yes

# if dropshift enabled:
ifeq ($(DROPSHIFT_ENABLE),yes)
	# auto-install caps word (unless user disables)
    CAPS_WORD_ENABLE ?= yes
    # set the dropshift compile flag
    OPT_DEFS += -DDROPSHIFT_ENABLE
endif
