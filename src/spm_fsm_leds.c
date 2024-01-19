/* Copyright (c) 2024-2024 Third Pole, Inc. All rights reserved. */

#include "hal_data.h"
#include "spm_fsm_leds.h"

static bsp_leds_t leds;

void set_led_off(void) {
    for(uint32_t i = 0; i < leds.led_count; i++) {
        g_ioport.p_api->pinWrite(leds.p_leds[i], IOPORT_LEVEL_HIGH);
    }
}

void set_led_red(void) {
    set_led_off();
    g_ioport.p_api->pinWrite(leds.p_leds[1], IOPORT_LEVEL_LOW);
}

void set_led_yellow(void) {
    set_led_off();
    g_ioport.p_api->pinWrite(leds.p_leds[2], IOPORT_LEVEL_LOW);
}

void set_led_green(void) {
    set_led_off();
    g_ioport.p_api->pinWrite(leds.p_leds[0], IOPORT_LEVEL_LOW);
}

void led_init(void) {
    R_BSP_LedsGet(&leds);
    if (0 == leds.led_count) {
        while(1);   // Error Trap
    }
    set_led_off();
}