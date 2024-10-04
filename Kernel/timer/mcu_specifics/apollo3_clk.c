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

#include "persistent_timer_commit/persistent_timer.h"
#include "timer.h"

static __nv _Atomic uint32_t current_ticks;

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
    am_hal_ctimer_config(1, (am_hal_ctimer_config_t*)&timer1_cfg);

    am_hal_ctimer_period_set(1, AM_HAL_CTIMER_TIMERA, ticks, 0);
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA1);
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA1);
}

void set_timer_xpr(uint16_t ticks)
{
    am_hal_ctimer_int_disable(AM_HAL_CTIMER_INT_TIMERA2);
    am_hal_ctimer_clear(2, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(2, (am_hal_ctimer_config_t*)&timer2_cfg);

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
    am_hal_ctimer_config(3, (am_hal_ctimer_config_t*)&timer3_cfg);

    am_hal_ctimer_period_set(3, AM_HAL_CTIMER_TIMERA, ticks, 0);
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA3);
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA3);
}

void __setup_rtc()
{
    am_hal_rtc_osc_select(AM_HAL_RTC_OSC_XT);
    am_hal_rtc_osc_enable();
    am_hal_rtc_time_12hour(false);
}

static void time_handler(void)
{
    current_ticks += 0x10000;
}

#ifdef WKUP_TIMER
static void wake_handler(void);
#endif
#ifdef XPR_TIMER
static void expiration_handler(void);
#endif
#ifdef PDC_TIMER
static void periodic_handler(void);
#endif

void __setup_clock()
{
    // am_hal_ctimer_int_register(CTIMER_INTEN_CTMRA0C0INT_Msk, )
    //  Clock System Setup
    am_hal_ctimer_int_register(CTIMER_INTEN_CTMRA0C0INT_Msk, time_handler);

#ifdef WKUP_TIMER
    am_hal_ctimer_int_register(CTIMER_INTEN_CTMRA1C0INT_Msk, wake_handler);
#endif
#ifdef XPR_TIMER
    am_hal_ctimer_int_register(CTIMER_INTEN_CTMRA2C0INT_Msk, expiration_handler);
#endif
#ifdef PDC_TIMER
    am_hal_ctimer_int_register(CTIMER_INTEN_CTMRA3C0INT_Msk, periodic_handler);
#endif
}

uint32_t __get_rtc_time()
{
    am_hal_rtc_time_t time = { 0 };
    am_hal_rtc_time_get(&time);

    uint32_t result = 0;
    result += time.ui32Hundredths * 10;
    result += time.ui32Second * 1000;
    result += time.ui32Minute * 1000 * 60;
    result += time.ui32Hour * 1000 * 3600;
    result += time.ui32DayOfMonth * 1000 * 3600;
    uint32_t ui32Year;
    uint32_t ui32Month;
    uint32_t ui32DayOfMonth;
    uint32_t ui32Hour;
    uint32_t ui32Minute;
    uint32_t ui32Second;
    uint32_t ui32Hundredths;

    return 0; // FIXME
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
    am_hal_ctimer_config(0, (am_hal_ctimer_config_t*)&timer0_cfg);

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

// the event that will be used to register the uart event
static isr_event_t timer_event;

#ifdef WKUP_TIMER
/* the timer interrupt handler */
// wkup_interrupt
static void wake_handler(void)
{
    stop_timer_wkup();

    if (!__EVENT_BUFFER_FULL(_pers_timer_get_nxt_thread(WKUP))) {
        timer_event.data = NULL;
        timer_event.size = 0;
        timer_event.timestamp = __get_time();
        //__on_time = TA2CCR0;
        __SIGNAL_EVENT(_pers_timer_get_nxt_thread(WKUP), &timer_event);

        // TODO: intermittent protection
        unpack_wkup_to_local();
        clear_wkup_status(_pers_timer_get_nxt_thread(WKUP));
        refresh_wkup_timers();
        _pers_timer_update_lock(WKUP);
        _pers_timer_commit(WKUP);
    }
}
#endif

#ifdef XPR_TIMER
/* the timer interrupt handler */
// xpr_interrupt
static void expiration_handler(void)
{
    /* stop timer */
    stop_timer_xpr();

    //__on_time = TA2CCR0;

    // TODO: intermittent protection
    unpack_xpr_to_local();
    //__stop_thread(__get_thread(nxt_xpr));
    __evict_thread(__get_thread(get_nxt_xpr()));
    clear_xpr_status(_pers_timer_get_nxt_thread(XPR));
    refresh_xpr_timers();

    // FIXME what is this? a GPIO?
    // P4OUT |= BIT2;
    // P4OUT &= ~BIT2;

    _pers_timer_update_lock(XPR);
    _pers_timer_commit(XPR);
}
#endif

#ifdef PDC_TIMER
/* the timer interrupt handler */
// pdc_interrupt
static void periodic_handler(void)
{
    /* stop timer */
    stop_timer_pdc();

    if (!__EVENT_BUFFER_FULL(_pers_timer_get_nxt_thread(PDC))) {
        timer_event.data = NULL;
        timer_event.size = 0;
        timer_event.timestamp = __get_time();
        //__on_time = TA1CCR0;
        __SIGNAL_EVENT(_pers_timer_get_nxt_thread(PDC), &timer_event);

        // TODO: intermittent protection
        // TODO: more clever implementation
        // FIXME what is thread_id? This was here in the original implementation, which means this wasn't tested at all
        uint8_t thread_id = get_nxt_pdc(); // FIXME this is a wild guess!!!
        __set_pdc_period(__get_thread(thread_id), 1 + __get_pdc_period(__get_thread(thread_id))); // FIXME there was a ++ here, but I don't know what it's meant to do as __get_pdc_period doesn't return an lvalue!

        unpack_pdc_to_local();
        clear_pdc_status(_pers_timer_get_nxt_thread(PDC));
        set_periodic_timer(_pers_timer_get_nxt_thread(PDC), __get_pdc_period(__get_thread(thread_id)) * __get_pdc_timer(__get_thread(thread_id)));
        refresh_pdc_timers();
        _pers_timer_update_lock(PDC);
        _pers_timer_commit(PDC);
    }
}
#endif

void am_ctimer_isr(void)
{
    uint32_t status = am_hal_ctimer_int_status_get(false);
    am_hal_ctimer_int_clear(status);
    am_hal_ctimer_int_service(status);
}
