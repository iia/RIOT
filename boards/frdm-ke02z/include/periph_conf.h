/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_frdm-ke02z NXP FRDM-KE02Z Board
 * @ingroup     boards
 * @brief       Support for the NXP FRDM-KE02Z
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the FRDM-KE02Z
 *
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Clock system configuration
 * @{
 */
//static const clock_config_t clock_config = {
    /*
     * This configuration results in the system running with the internal clock
     * with the following clock frequencies:
     * Core:   8 MHz
     * Bus:    8 MHz
     * Flash:  8 MHz
     */
    //.clkdiv1            = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(0),
    /* unsure if this RTC load cap configuration is correct */
    //.rtc_clc            = RTC_CR_SC8P_MASK | RTC_CR_SC4P_MASK,
    /* Use the 32 kHz system oscillator output as ERCLK32K. */
    //.osc32ksel          = SIM_SOPT1_OSC32KSEL(0),
    //.clock_flags =
    //    KINETIS_CLOCK_RTCOSC_EN |
    //    KINETIS_CLOCK_USE_FAST_IRC |
    //    KINETIS_CLOCK_MCGIRCLK_EN | /* Used for LPUART clocking */
    //    KINETIS_CLOCK_MCGIRCLK_STOP_EN |
    //    0,
    /* Using LIRC8M mode by default */
    //.default_mode       = KINETIS_MCG_MODE_LIRC8M,
    /* The crystal connected to EXTAL0 is 32.768 kHz */
    //.erc_range          = KINETIS_MCG_ERC_RANGE_LOW,
    //.osc_clc            = 0, /* no load cap configuration, rtc_clc overrides this value on KL43Z */
    //.fcrdiv             = MCG_SC_FCRDIV(0), /* LIRC_DIV1 divide by 1 => 8 MHz */
    //.lirc_div2          = MCG_MC_LIRC_DIV2(0), /* LIRC_DIV2 divide by 1 => 8 MHz */
//};
#define CLOCK_CORECLOCK              (16000000UL)
//#define CLOCK_MCGIRCLK               (8000000ul)
#define CLOCK_BUSCLOCK               (CLOCK_CORECLOCK / 1)
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define STDIO_UART_DEV       (UART_DEV(0))

#define STDIO_UART_BAUDRATE (115200UL)

static const uart_conf_t uart_config[] = {
    {
        .dev    = UART0,
        .freq   = CLOCK_CORECLOCK,
        /*
        .pin_rx = GPIO_PIN(PORT_A,  1),
        .pin_tx = GPIO_PIN(PORT_A,  2),
        .pcr_rx = PORT_PCR_MUX(2),
        .pcr_tx = PORT_PCR_MUX(2),
        */
        .irqn   = UART0_IRQn,
        .scgc_addr = &SIM->SCGC,
        .scgc_bit = SIM_SCGC_UART0_SHIFT,
        .mode   = UART_MODE_8N1,
        .type   = KINETIS_UART,
    },
};

#define UART_NUMOF          ARRAY_SIZE(uart_config)
//#define UART_0_ISR        isr_uart0

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
