#include "spm_fsm_thread0.h"
#include "spm_fsm.h"

int pb_happened = 0;

static void spm_isr_init(void) {
    ssp_err_t status;

    status = g_external_irq0.p_api->open(g_external_irq0.p_ctrl,
                                         g_external_irq0.p_cfg);
    if (status != SSP_SUCCESS) {
        while(1);
    }

    status = g_external_irq0.p_api->enable(g_external_irq0.p_ctrl);
    if (status != SSP_SUCCESS) {
        while(1);
    }
}

/* SPM_Thread entry function */
void spm_fsm_thread0_entry(void) {
    spm_isr_init();
    spm_fsm_init();
    while(1) {
      if (pb_happened) {
          pb_happened = 0;
          spm_fsm_proc_evt(SPM_FSM_EVT_PB, NULL);
      }
      tx_thread_sleep(20);
    }
}

void pb_callback(external_irq_callback_args_t *p_args) {
    pb_happened = 1;
}