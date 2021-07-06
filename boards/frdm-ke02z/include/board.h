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
 * @brief       Board specific definitions for the FRDM-KE02Z
 *
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
//#define LED0_PIN            GPIO_PIN(PORT_D,  5)
/** @} */

/**
 * @brief   Initialize board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
