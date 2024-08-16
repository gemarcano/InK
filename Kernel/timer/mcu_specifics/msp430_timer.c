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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#include "mcu_specifics/clk.h"
#include "persistent_timer_commit/persistent_timer.h"
#include "timer.h"

// ISR register section
/*************************************************************************************************************/

// the event that will be used to register the uart event
isr_event_t timer_event;

#ifdef WKUP_TIMER
/* the timer interrupt handler */
// wkup_interrupt
__interrupt(TIMER0_A0_VECTOR)
{
    /* stop timer */
    TA0CCTL0 = 0x00;
    TA0CTL = 0;

    if (!__EVENT_BUFFER_FULL(_pers_timer_get_nxt_thread(WKUP))) {
        timer_event.ISRtype = 0;
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

    /* turn on CPU */
    __bic_SR_register_on_exit(LPM3_bits);
}
#endif

#ifdef XPR_TIMER
/* the timer interrupt handler */
// xpr_interrupt
_interrupt(TIMER2_A0_VECTOR)
{
    /* stop timer */
    TA2CCTL0 = 0x00;
    TA2CTL = 0;

    //__on_time = TA2CCR0;

    // TODO: intermittent protection
    unpack_xpr_to_local();
    //__stop_thread(__get_thread(nxt_xpr));
    __evict_thread(__get_thread(nxt_xpr));
    clear_xpr_status(_pers_timer_get_nxt_thread(XPR));
    refresh_xpr_timers();
    P4OUT |= BIT2;
    P4OUT &= ~BIT2;
    _pers_timer_update_lock(XPR);
    _pers_timer_commit(XPR);
    /* turn on CPU */
    __bic_SR_register_on_exit(LPM3_bits);
}
#endif

#ifdef PDC_TIMER
/* the timer interrupt handler */
// pdc_interrupt
__interrupt(TIMER1_A0_VECTOR)
{
    /* stop timer */
    TA1CCTL0 = 0x00;
    TA1CTL = 0;

    if (!__EVENT_BUFFER_FULL(_pers_timer_get_nxt_thread(PDC))) {
        timer_event.ISRtype = 0;
        timer_event.data = NULL;
        timer_event.size = 0;
        timer_event.timestamp = __get_time();
        //__on_time = TA1CCR0;
        __SIGNAL_EVENT(_pers_timer_get_nxt_thread(PDC), &timer_event);

        // TODO: intermittent protection
        // TODO: more clever implementation
        __set_pdc_period(__get_thread(thread_id), ++__get_pdc_period(__get_thread(thread_id)));

        unpack_to_local();
        clear_pdc_status(_pers_timer_get_nxt_thread(PDC));
        set_persiodic_timer(_pers_timer_get_nxt_thread(PDC),__get_pdc_period(__get_thread(thread_id)*__get_pdc_timer(__get_thread(thread_id)));
        refresh_pdc_timers();
        _pers_timer_update_lock(PDC);
        _pers_timer_commit(PDC);
    }

    /* turn on CPU */
    __bic_SR_register_on_exit(LPM3_bits);
}
#endif
