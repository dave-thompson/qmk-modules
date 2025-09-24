# module enabled by default, unless user explicitly disables it
LUMBERJACK_ENABLE ?= yes

ifeq ($(strip $(LUMBERJACK_ENABLE)),yes)

	# add source files
	SRC += lumberjack_utils.c
	SRC += lumberjack_color_queue.c
	SRC += lumberjack_config.c
	SRC += lumberjack_tracking.c
	SRC += lumberjack_logging.c

	# enable required features
	CONSOLE_ENABLE = yes # compulsory
	KEYCODE_STRING_ENABLE ?= yes # enabled unless user disables

	# set the Lumberjack compile flag
	OPT_DEFS += -DLUMBERJACK_ENABLE

endif
