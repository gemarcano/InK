// This file is part of InK.
//
// author = "dpatoukas"
// maintainer = "dpatoukas"
// email = "dpatoukas@gmail.com"
//
// copyright = "Copyright 2018 Delft University of Technology"
// license = "LGPL"
// version = "3.0"
// status = "Production"
//
//
// InK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.//clk.c

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"

#include "clk.h"
#include "ink.h"

static __nv uint32_t current_ticks;

// This is used by get_time
static const am_hal_ctimer_config_t timer0_cfg = {
    // Don't link timers.
    0,
    // Set up Timer0A.
    (AM_HAL_CTIMER_FN_REPEAT | AM_HAL_CTIMER_INT_ENABLE | AM_HAL_CTIMER_XT_32_768KHZ),
    // No configuration for Timer0B.
    0,
};

// This is used for wake timer
static const am_hal_ctimer_config_t timer1_cfg = {
    // Don't link timers.
    0,
    // Set up Timer1A.
    (AM_HAL_CTIMER_FN_REPEAT | AM_HAL_CTIMER_INT_ENABLE | AM_HAL_CTIMER_XT_32_768KHZ),
    // No configuration for Timer1B.
    0,
};

// This is used for expiration timer
static const am_hal_ctimer_config_t timer2_cfg = {
    // Don't link timers.
    0,
    // Set up Timer2A.
    (AM_HAL_CTIMER_FN_REPEAT | AM_HAL_CTIMER_INT_ENABLE | AM_HAL_CTIMER_XT_32_768KHZ),
    // No configuration for Timer2B.
    0,
};

// This is used for periodic timer
static const am_hal_ctimer_config_t timer3_cfg = {
    // Don't link timers.
    0,
    // Set up Timer3A.
    (AM_HAL_CTIMER_FN_REPEAT | AM_HAL_CTIMER_INT_ENABLE | AM_HAL_CTIMER_XT_32_768KHZ),
    // No configuration for Timer3B.
    0,
};

void set_timer_wkup(uint16_t ticks)
{
    am_hal_ctimer_int_disable(AM_HAL_CTIMER_INT_TIMERA1);
    am_hal_ctimer_clear(1, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(1, &timer1_cfg);

    am_hal_ctimer_period_set(1, AM_HAL_CTIMER_TIMERA, ticks, 0);
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA1);
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA1);
}

void set_timer_xpr(uint16_t ticks)
{
    am_hal_ctimer_int_disable(AM_HAL_CTIMER_INT_TIMERA2);
    am_hal_ctimer_clear(2, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(2, &timer2_cfg);

    am_hal_ctimer_period_set(2, AM_HAL_CTIMER_TIMERA, ticks, 0);
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA2);
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA2);
}

void stop_timer_xpr()
{
    am_hal_ctimer_int_disable(AM_HAL_CTIMER_INT_TIMERA2);
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA2);
}

void stop_timer_wkup()
{
    am_hal_ctimer_int_disable(AM_HAL_CTIMER_INT_TIMERA1);
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA1);
}

void stop_timer_pdc()
{
    am_hal_ctimer_int_disable(AM_HAL_CTIMER_INT_TIMERA3);
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA3);
}

void set_timer_pdc(uint16_t ticks)
{
    am_hal_ctimer_int_disable(AM_HAL_CTIMER_INT_TIMERA3);
    am_hal_ctimer_clear(3, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(3, &timer3_cfg);

    am_hal_ctimer_period_set(3, AM_HAL_CTIMER_TIMERA, ticks, 0);
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA3);
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA3);
}

void __setup_rtc()
{
}

void __setup_clock()
{
    // am_hal_ctimer_int_register(CTIMER_INTEN_CTMRA0C0INT_Msk, )
    //  Clock System Setup
}
uint32_t __get_rtc_time()
{
    uint32_t buff;
    uint32_t current_time;
}

/*
 * start the __get_time interface
 */
void __get_time_init()
{
    // configure rtc interface
    __setup_rtc();

    // get the current time from rtc;
    current_ticks = __get_rtc_time();

    am_hal_ctimer_int_disable(AM_HAL_CTIMER_INT_TIMERA0);
    am_hal_ctimer_clear(0, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(0, &timer0_cfg);

    am_hal_ctimer_period_set(0, AM_HAL_CTIMER_TIMERA, 256, 128);
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA0);
    NVIC_EnableIRQ(CTIMER_IRQn);
}

/*
 * stop the __get_time interface
 */
void __get_time_stop()
{
    am_hal_ctimer_int_disable(AM_HAL_CTIMER_INT_TIMERA0);
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);
}

/*
 * return current ticks + number of times the current timer has overflown
 * TASSEL_SMLK:1Mhz -> time will be reported in milliseconds (us)
 */
uint32_t __get_time()
{
    uint32_t tmp = am_hal_ctimer_read(0, AM_HAL_CTIMER_TIMERA);

    return (tmp * 10 + current_ticks); // current_ticks/1000;
}

void am_ctimer_isr(void)
{
    // FIXME I need to differentiate between interrupts
    current_ticks += 0x10000;
    uint32_t status = am_hal_ctimer_int_status_get(false);
    am_hal_ctimer_int_clear(status);
    am_hal_ctimer_int_service(status);
}
