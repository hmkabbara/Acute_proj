/* generated HAL source file - do not edit */
#include "hal_data.h"
#if (4) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_external_irq0) && !defined(SSP_SUPPRESS_ISR_ICU11)
SSP_VECTOR_DEFINE(icu_irq_isr, ICU, IRQ11);
#endif
#endif
static icu_instance_ctrl_t g_external_irq0_ctrl;
static const external_irq_cfg_t g_external_irq0_cfg =
{
    .channel             = 11,
    .trigger             = EXTERNAL_IRQ_TRIG_FALLING,
    .filter_enable       = true,
    .pclk_div            = EXTERNAL_IRQ_PCLK_DIV_BY_64,
    .autostart           = true,
    .p_callback          = pb_callback,
    .p_context           = &g_external_irq0,
    .p_extend            = NULL,
    .irq_ipl             = (4),
};
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq0 = 
{
    .p_ctrl        = &g_external_irq0_ctrl,
    .p_cfg         = &g_external_irq0_cfg,
    .p_api         = &g_external_irq_on_icu
};
void g_hal_init(void) {
g_common_init();
}
