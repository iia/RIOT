/*
 * Copyright 2015 Ludwig Knüpfer
 *           2015 Christian Mehlis
 *           2016-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dht
 * @{
 *
 * @file
 * @brief       Device driver implementation for the DHT 11 and 22
 *              temperature and humidity sensor
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "log.h"
#include "assert.h"
#include "xtimer.h"
#include "timex.h"
#include "periph/gpio.h"

#include "dht.h"
#include "dht_params.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* Every pulse send by the DHT longer than 40µs is interpreted as 1 */
#define PULSE_WIDTH_THRESHOLD       (40U)
/* If an expected pulse is not detected within 1000µs, something is wrong */
#define TIMEOUT                     (1000U)

static inline void _reset(const dht_t *dev)
{
    gpio_init(dev->pin, GPIO_OUT);
    gpio_set(dev->pin);

}

static int _read(uint16_t *dest, gpio_t pin, int bits)
{
    DEBUG("read\n");
    uint16_t res = 0;

    for (int i = 0; i < bits; i++) {
        uint32_t start, end;
        res <<= 1;
        /* measure the length between the next rising and falling flanks (the
         * time the pin is high - smoke up :-) */
        unsigned counter = 0;
        while (!gpio_read(pin)) {
            if (counter++ >= TIMEOUT) {
                return -1;
            }
            xtimer_usleep(1);
        }
        start = xtimer_now_usec();

        counter = 0;
        while (gpio_read(pin)) {
            if (counter++ >= TIMEOUT) {
                return -1;
            }
            xtimer_usleep(1);
        }
        end = xtimer_now_usec();
        /* if the high phase was more than 40us, we got a 1 */
        if ((end - start) > PULSE_WIDTH_THRESHOLD) {
            res |= 0x0001;
        }
    }

    *dest = res;
    return 0;
}

int dht_init(dht_t *dev, const dht_params_t *params)
{
    DEBUG("dht_init\n");

    /* check parameters and configuration */
    assert(dev && params &&
           ((params->type == DHT11) || (params->type == DHT22) || (params->type == DHT21)));

    *dev = *params;

    _reset(dev);

    xtimer_usleep(2000 * US_PER_MS);

    DEBUG("dht_init: success\n");
    return DHT_OK;
}

int dht_read(const dht_t *dev, int16_t *temp, int16_t *hum)
{
    uint16_t csum, sum;
    uint16_t raw_hum, raw_temp;

    assert(dev && temp && hum);

    /* send init signal to device */
    gpio_clear(dev->pin);
    xtimer_usleep(20 * US_PER_MS);
    gpio_set(dev->pin);
    xtimer_usleep(40);

    /* sync on device */
    gpio_init(dev->pin, dev->in_mode);
    unsigned counter = 0;
    while (!gpio_read(dev->pin)) {
        if (counter++ > TIMEOUT) {
            _reset(dev);
            return DHT_TIMEOUT;
        }
        xtimer_usleep(1);
    }

    counter = 0;
    while (gpio_read(dev->pin)) {
        if (counter++ > TIMEOUT) {
            _reset(dev);
            return DHT_TIMEOUT;
        }
        xtimer_usleep(1);
    }

    /*
     * data is read in sequentially, highest bit first:
     *  40 .. 24  23   ..   8  7  ..  0
     * [humidity][temperature][checksum]
     */

    /* read the humidity, temperature, and checksum bits */
    if (_read(&raw_hum, dev->pin, 16)) {
        _reset(dev);
        return DHT_TIMEOUT;
    }

    if (_read(&raw_temp, dev->pin, 16)) {
        _reset(dev);
        return DHT_TIMEOUT;
    }

    if (_read(&csum, dev->pin, 8)) {
        _reset(dev);
        return DHT_TIMEOUT;
    }

    /* Bring device back to defined state - so we can trigger the next reading
     * by pulling the data pin low again */
    _reset(dev);

    /* validate the checksum */
    sum = (raw_temp >> 8) + (raw_temp & 0xff) + (raw_hum >> 8) + (raw_hum & 0xff);
    if ((sum != csum) || (csum == 0)) {
        DEBUG("error: checksum invalid\n");
        return DHT_NOCSUM;
    }

    /* parse the RAW values */
    DEBUG("RAW values: temp: %7i hum: %7i\n", (int)raw_temp, (int)raw_hum);
    switch (dev->type) {
        case DHT11:
            *temp = (int16_t)((raw_temp >> 8) * 10);
            *hum = (int16_t)((raw_hum >> 8) * 10);
            break;
        case DHT22:
            *hum = (int16_t)raw_hum;
            /* if the high-bit is set, the value is negative */
            if (raw_temp & 0x8000) {
                *temp = (int16_t)((raw_temp & ~0x8000) * -1);
            }
            else {
                *temp = (int16_t)raw_temp;
            }
            break;
        default:
            return DHT_NODEV;      /* this should never be reached */
    }

    return DHT_OK;
}
