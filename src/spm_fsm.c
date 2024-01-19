/* Copyright (c) 2024-2024 Third Pole, Inc. All rights reserved. */
#include <stddef.h>
#include "hal_data.h"
#include "spm_fsm.h"
#include "spm_fsm_leds.h"

/*
 * Use 3 color LEDs to indicate states
 * STATE    |   GREEN   YELLOW  RED
 * ---------+----------------------
 * reset    |   off     off     off
 * post     |   off     off     on
 * selftest |   off     on      off
 * idle     |   on      off     off
 * 
 * push button event causes state change.
 * reset => post => selftest => idle => reset and respeat ...
 */

/* info for fsm */
typedef struct {
    spm_fsm_state_e cur_state;
} spm_fsm_info_t;

static spm_fsm_info_t spm_fsm_info;

static spm_fsm_state_e prv_reset_state_pb_evt_fn(void *data) {
    set_led_red();
    return SPM_FSM_STATE_POST;
}

static spm_fsm_state_e prv_post_state_pb_evt_fn(void *data) {
    set_led_yellow();
    return SPM_FSM_STATE_SELFTEST;
}

static spm_fsm_state_e prv_selftest_state_pb_evt_fn(void *data) {
    set_led_green();
    return SPM_FSM_STATE_IDLE;
}

static spm_fsm_state_e prv_idle_state_pb_evt_fn(void *data) {
    set_led_off();
    return SPM_FSM_STATE_RESET;
}

typedef struct {
    spm_fsm_state_e state;
    spm_fsm_evt_e evt;
    spm_fsm_state_e (*fn)(void *data);
} spm_fsm_tbl_ent_t;

static spm_fsm_tbl_ent_t prv_spm_fsm_tbl[] = {
    { SPM_FSM_STATE_RESET   , SPM_FSM_EVT_PB, prv_reset_state_pb_evt_fn    },
    { SPM_FSM_STATE_POST    , SPM_FSM_EVT_PB, prv_post_state_pb_evt_fn     },
    { SPM_FSM_STATE_SELFTEST, SPM_FSM_EVT_PB, prv_selftest_state_pb_evt_fn },
    { SPM_FSM_STATE_IDLE    , SPM_FSM_EVT_PB, prv_idle_state_pb_evt_fn     },
};

void spm_fsm_init(void) {
    led_init();

    spm_fsm_info.cur_state = SPM_FSM_STATE_RESET;
}

spm_fsm_state_e spm_fsm_get_cur_state(void) {
    return spm_fsm_info.cur_state;
}

spm_fsm_state_e spm_fsm_proc_evt(spm_fsm_evt_e evt, void *data) {
    size_t i, num = sizeof(prv_spm_fsm_tbl)/sizeof(prv_spm_fsm_tbl[0]);
    spm_fsm_tbl_ent_t *p;

    for (i = 0; i < num; i++) {
        p = &prv_spm_fsm_tbl[i];
        if (p->state == spm_fsm_info.cur_state && p->evt == evt && p->fn) {
            spm_fsm_info.cur_state = p->fn(data);
            break;
        }
    }

    return spm_fsm_info.cur_state;
}
