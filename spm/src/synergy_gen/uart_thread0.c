/* generated thread source file - do not edit */
#include "uart_thread0.h"

TX_THREAD uart_thread0;
                void uart_thread0_create(void);
                static void uart_thread0_func(ULONG thread_input);
                static uint8_t uart_thread0_stack[1024] BSP_PLACE_IN_SECTION_V2(".stack.uart_thread0") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
                void tx_startup_err_callback(void * p_instance, void * p_data);
                void tx_startup_common_init(void);
#if (6) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer1) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_SCI0_RXI)
#define DTC_ACTIVATION_SRC_ELC_EVENT_SCI0_RXI
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_0) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_0);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0
#endif
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_1) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_1);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1
#endif
#endif
#endif

dtc_instance_ctrl_t g_transfer1_ctrl;
transfer_info_t g_transfer1_info =
{
    .dest_addr_mode      = TRANSFER_ADDR_MODE_INCREMENTED,
    .repeat_area         = TRANSFER_REPEAT_AREA_DESTINATION,
    .irq                 = TRANSFER_IRQ_END,
    .chain_mode          = TRANSFER_CHAIN_MODE_DISABLED,
    .src_addr_mode       = TRANSFER_ADDR_MODE_FIXED,
    .size                = TRANSFER_SIZE_1_BYTE,
    .mode                = TRANSFER_MODE_NORMAL,
    .p_dest              = (void *) NULL,
    .p_src               = (void const *) NULL,
    .num_blocks          = 0,
    .length              = 0,
};
const transfer_cfg_t g_transfer1_cfg =
{
    .p_info              = &g_transfer1_info,
    .activation_source   = ELC_EVENT_SCI0_RXI,
    .auto_enable         = false,
    .p_callback          = NULL,
    .p_context           = &g_transfer1,
    .irq_ipl             = (6)
};
/* Instance structure to use this module. */
const transfer_instance_t g_transfer1 = 
{
    .p_ctrl        = &g_transfer1_ctrl,
    .p_cfg         = &g_transfer1_cfg,
    .p_api         = &g_transfer_on_dtc
};
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer0) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_SCI0_TXI)
#define DTC_ACTIVATION_SRC_ELC_EVENT_SCI0_TXI
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_0) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_0);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0
#endif
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_1) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_1);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1
#endif
#endif
#endif

dtc_instance_ctrl_t g_transfer0_ctrl;
transfer_info_t g_transfer0_info =
{
    .dest_addr_mode      = TRANSFER_ADDR_MODE_FIXED,
    .repeat_area         = TRANSFER_REPEAT_AREA_SOURCE,
    .irq                 = TRANSFER_IRQ_END,
    .chain_mode          = TRANSFER_CHAIN_MODE_DISABLED,
    .src_addr_mode       = TRANSFER_ADDR_MODE_INCREMENTED,
    .size                = TRANSFER_SIZE_1_BYTE,
    .mode                = TRANSFER_MODE_NORMAL,
    .p_dest              = (void *) NULL,
    .p_src               = (void const *) NULL,
    .num_blocks          = 0,
    .length              = 0,
};
const transfer_cfg_t g_transfer0_cfg =
{
    .p_info              = &g_transfer0_info,
    .activation_source   = ELC_EVENT_SCI0_TXI,
    .auto_enable         = false,
    .p_callback          = NULL,
    .p_context           = &g_transfer0,
    .irq_ipl             = (BSP_IRQ_DISABLED)
};
/* Instance structure to use this module. */
const transfer_instance_t g_transfer0 = 
{
    .p_ctrl        = &g_transfer0_ctrl,
    .p_cfg         = &g_transfer0_cfg,
    .p_api         = &g_transfer_on_dtc
};
#if SCI_UART_CFG_RX_ENABLE
#if (6) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_uart0) && !defined(SSP_SUPPRESS_ISR_SCI0)
SSP_VECTOR_DEFINE_CHAN(sci_uart_rxi_isr, SCI, RXI, 0);
#endif
#endif
#endif
#if SCI_UART_CFG_TX_ENABLE
#if (6) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_uart0) && !defined(SSP_SUPPRESS_ISR_SCI0)
SSP_VECTOR_DEFINE_CHAN(sci_uart_txi_isr, SCI, TXI, 0);
#endif
#endif
#if (6) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_uart0) && !defined(SSP_SUPPRESS_ISR_SCI0)
SSP_VECTOR_DEFINE_CHAN(sci_uart_tei_isr, SCI, TEI, 0);
#endif
#endif
#endif
#if SCI_UART_CFG_RX_ENABLE
#if (6) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_uart0) && !defined(SSP_SUPPRESS_ISR_SCI0)
SSP_VECTOR_DEFINE_CHAN(sci_uart_eri_isr, SCI, ERI, 0);
#endif
#endif
#endif
            sci_uart_instance_ctrl_t     g_uart0_ctrl;

            /** UART extended configuration for UARTonSCI HAL driver */
            const uart_on_sci_cfg_t g_uart0_cfg_extend =
            {
                .clk_src                 = SCI_CLK_SRC_INT,
                .baudclk_out             = false,
                .rx_edge_start           = true,
                .noisecancel_en          = false,
                .p_extpin_ctrl           = NULL,
                .bitrate_modulation      = true,
                .rx_fifo_trigger         = SCI_UART_RX_FIFO_TRIGGER_MAX,
                .baud_rate_error_x_1000  = (uint32_t)(2.0*1000),
		 .uart_comm_mode          = UART_MODE_RS232,		 
		 .uart_rs485_mode         = UART_RS485_HD,
		 .rs485_de_pin            = IOPORT_PORT_09_PIN_14,
            };

            /** UART interface configuration */
            const uart_cfg_t g_uart0_cfg =
            {
                .channel             = 0,
                .baud_rate           = 115200,
                .data_bits           = UART_DATA_BITS_8,
                .parity              = UART_PARITY_OFF,
                .stop_bits           = UART_STOP_BITS_1,
                .ctsrts_en           = false,
                .p_callback          = NULL,
                .p_context           = &g_uart0,
                .p_extend            = &g_uart0_cfg_extend,
#define SYNERGY_NOT_DEFINED (1)                        
#if (SYNERGY_NOT_DEFINED == g_transfer0)
                .p_transfer_tx       = NULL,
#else
                .p_transfer_tx       = &g_transfer0,
#endif            
#if (SYNERGY_NOT_DEFINED == g_transfer1)
                .p_transfer_rx       = NULL,
#else
                .p_transfer_rx       = &g_transfer1,
#endif   
#undef SYNERGY_NOT_DEFINED            
                .rxi_ipl             = (6),
                .txi_ipl             = (6),
                .tei_ipl             = (6),
                .eri_ipl             = (6),
            };

/* Instance structure to use this module. */
const uart_instance_t g_uart0 = 
{
    .p_ctrl        = &g_uart0_ctrl,
    .p_cfg         = &g_uart0_cfg,
    .p_api         = &g_uart_on_sci
};
#if defined(__ICCARM__)
#define g_sf_comms0_err_callback_WEAK_ATTRIBUTE
#pragma weak g_sf_comms0_err_callback  = g_sf_comms0_err_callback_internal
#elif defined(__GNUC__)
#define g_sf_comms0_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_comms0_err_callback_internal")))
#endif
void g_sf_comms0_err_callback(void * p_instance, void * p_data) g_sf_comms0_err_callback_WEAK_ATTRIBUTE;
static sf_uart_comms_instance_ctrl_t g_sf_comms0_ctrl;
static const sf_uart_comms_cfg_t g_sf_comms0_cfg_extend =
{
	.p_lower_lvl_uart = &g_uart0,
};
static const sf_comms_cfg_t g_sf_comms0_cfg =
{
    .p_extend = &g_sf_comms0_cfg_extend,
};
/* Instance structure to use this module. */
const sf_comms_instance_t g_sf_comms0 = 
{
    .p_ctrl        = &g_sf_comms0_ctrl,
    .p_cfg         = &g_sf_comms0_cfg,
    .p_api         = &g_sf_comms_on_sf_uart_comms
};
 /*******************************************************************************************************************//**
  * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
  *             with the prototype below.
  *             - void g_sf_comms0_err_callback(void * p_instance, void * p_data)
  *
  * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
  **********************************************************************************************************************/
 void g_sf_comms0_err_callback_internal(void * p_instance, void * p_data);
 void g_sf_comms0_err_callback_internal(void * p_instance, void * p_data)
 {
    /** Suppress compiler warning for not using parameters. */
     SSP_PARAMETER_NOT_USED(p_instance);
     SSP_PARAMETER_NOT_USED(p_data);

    /** An error has occurred. Please check function arguments for more information. */
     BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
 }
/*******************************************************************************************************************//**
* @brief     Initialization function that the user can choose to have called automatically during thread entry.
*            The user can call this function at a later time if desired using the prototype below.
*            - void sf_comms_init0(void)
**********************************************************************************************************************/
void sf_comms_init0(void)
{
    ssp_err_t ssp_err_g_sf_comms0;
    ssp_err_g_sf_comms0 = g_sf_comms0.p_api->open(g_sf_comms0.p_ctrl, g_sf_comms0.p_cfg);
    if (SSP_SUCCESS != ssp_err_g_sf_comms0)
    {
        g_sf_comms0_err_callback((void *)&g_sf_comms0,&ssp_err_g_sf_comms0);
    }          
}
extern bool         g_ssp_common_initialized;
                extern uint32_t     g_ssp_common_thread_count;
                extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

                void uart_thread0_create (void)
                {
                    /* Increment count so we will know the number of ISDE created threads. */
                    g_ssp_common_thread_count++;

                    /* Initialize each kernel object. */
                    

                    UINT err;
                    err = tx_thread_create(
                        &uart_thread0,
                        (CHAR *)"UART Thread",
                        uart_thread0_func,
                        (ULONG) NULL,
                        &uart_thread0_stack,
                        1024,
                        1,
                        1,
                        1,
                        TX_AUTO_START
                    );
                    if (TX_SUCCESS != err) {
                        tx_startup_err_callback(&uart_thread0, 0);
                    }
                }

                static void uart_thread0_func (ULONG thread_input) {
                    /* Not currently using thread_input. */
                    SSP_PARAMETER_NOT_USED(thread_input);

                    /* Initialize common components */
                    tx_startup_common_init();

                    /* Initialize each module instance. */
                    /** Call initialization function if user has selected to do so. */
            #if (1)
                sf_comms_init0();
            #endif

                    /* Enter user code for this thread. */
                    uart_thread0_entry();
                }
