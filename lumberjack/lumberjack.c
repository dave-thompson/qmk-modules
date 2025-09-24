#ifdef LUMBERJACK_ENABLE

#include "quantum.h"
#include "lumberjack_utils.h"
#include "lumberjack_color_queue.h"
#include "lumberjack_config.h"
#include "lumberjack_tracking.h"
#include "lumberjack_logging.h"

///////////////////////////////////////////////////////////////////////////////
//
// State
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    bool active;                // has there been a keypress in last 60 secs?
    uint16_t last_event_time;   // time of last key event
} lumberjack_state_t;

static lumberjack_state_t state = {0};


// Update state if newly idle (60 seconds from last key event)
static void update_state_if_idle(void) {
    if (state.active) {
        uint16_t idle_time = timer_read() - state.last_event_time;
        if (idle_time > LUMBERJACK_MAX_DELTA) state.active = false;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// QMK Hooks: Processing Keys
//
///////////////////////////////////////////////////////////////////////////////

bool pre_process_record_lumberjack(uint16_t current_keycode,
                                   keyrecord_t *record) {

    // track keypress
    keypress_t keypress_data = lumberjack_track_key(current_keycode, record);

    // choose keycode to be logged
    uint16_t log_keycode;
    if (keypress_data.keycode != 0) {
        // if tracking successful, use tracked keycode
        log_keycode = keypress_data.keycode;
    }
    else { // else fallback to current_keycode
        log_keycode = current_keycode;
    }

    // calculate delta since last event
    uint16_t delta = record->event.time - state.last_event_time;
    state.last_event_time = record->event.time;
    // if returning from idle, do not log delta (as likely overflowed)
    if (!state.active) {
        delta = UINT16_MAX;
        state.active = true;
    }

    // log event
    lumberjack_log_event(&keypress_data, log_keycode, delta,
                         record->event.pressed);

    return true;
}


bool process_record_lumberjack(uint16_t current_keycode, keyrecord_t *record) {
    return !lumberjack_toggle_if_lumberj_key(current_keycode, record);
}


///////////////////////////////////////////////////////////////////////////////
//
// QMK Hooks: Initialisation & Housekeeping
//
///////////////////////////////////////////////////////////////////////////////

void keyboard_post_init_lumberjack(void) {
    lumberjack_init_colors();
}


void housekeeping_task_lumberjack(void) {
    update_state_if_idle();
}


#endif // LUMBERJACK_ENABLE
