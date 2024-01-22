/*
 * Copyright (c) 2024-2024 Third Pole, Inc. All rights reserved.
 */

#ifndef __SPM_FSM_H__
#define __SPM_FSM_H__

typedef struct spm_fsm_inst_t_ spm_fsm_hdl_t;

typedef enum {
    SPM_FSM_STATE_RESET,
    SPM_FSM_STATE_POST,
    SPM_FSM_STATE_SELFTEST,
    SPM_FSM_STATE_IDLE,
    SPM_FSM_STATE_COUNT
} spm_fsm_state_e;

typedef enum {
    SPM_FSM_EVT_PB, /* temp: push button 1 event */
    SPM_FSM_EVT_NUM,
} spm_fsm_evt_e;

extern void spm_fsm_init(void);
extern spm_fsm_state_e spm_fsm_get_cur_state(void);
extern spm_fsm_state_e spm_fsm_proc_evt(spm_fsm_evt_e evt, void *data);

#endif