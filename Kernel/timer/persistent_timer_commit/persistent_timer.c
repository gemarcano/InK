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
#include "persistent_timer.h"

#include <stddef.h>

/// Used for the timer commit state machine
typedef enum {
    TIMER_DIRTY,
    TIMER_CLEAN
} tmr_st;

static __nv _Atomic tmr_st pdc_tstatus = TIMER_CLEAN;
static __nv _Atomic tmr_st xpr_tstatus = TIMER_CLEAN;
static __nv _Atomic tmr_st wkup_tstatus = TIMER_CLEAN;

// Represents all of the timer state and stores it in non-volatile memory.
static __nv pers_timers_t pers_timers;

void _pers_timer_init(void)
{
    for (uint8_t i = 0; i < MAX_TIMED_THREADS; i++) {
        pers_timers.wkup_timing[i].persistent.status = NOT_USED;
        pers_timers.xpr_timing[i].persistent.status = NOT_USED;
        pers_timers.pdc_timing[i].persistent.status = NOT_USED;
    }
}

void _pers_timer_update_data(uint8_t idx, ink_time_interface_t interface, uint32_t time_data)
{
    // update the persistent timer dirty buffer
    switch (interface) {
    case WKUP:
        pers_timers.wkup_timing[idx].dirty.time = time_data;
        pers_timers.wkup_timing[idx].state = DIRTY;
        break;
    case XPR:
        pers_timers.xpr_timing[idx].dirty.time = time_data;
        pers_timers.xpr_timing[idx].state = DIRTY;
        break;
    case PDC:
        pers_timers.pdc_timing[idx].dirty.time = time_data;
        pers_timers.pdc_timing[idx].state = DIRTY;
        break;
    }
}

void _pers_timer_update_thread_id(uint8_t idx, ink_time_interface_t interface, uint8_t thread_id)
{
    // update the persistent timer dirty buffer
    switch (interface) {
    case WKUP:
        pers_timers.wkup_timing[idx].dirty.thread_id = thread_id;
        pers_timers.wkup_timing[idx].state = DIRTY;
        break;
    case XPR:
        pers_timers.xpr_timing[idx].dirty.thread_id = thread_id;
        pers_timers.xpr_timing[idx].state = DIRTY;
        break;
    case PDC:
        pers_timers.pdc_timing[idx].dirty.thread_id = thread_id;
        pers_timers.pdc_timing[idx].state = DIRTY;
        break;
    }
}

void _pers_timer_update_status(uint8_t idx, ink_time_interface_t interface, used_st status)
{
    // update the persistent timer dirty buffer
    switch (interface) {
    case WKUP:
        pers_timers.wkup_timing[idx].dirty.status = status;
        pers_timers.wkup_timing[idx].state = DIRTY;
        break;
    case XPR:
        pers_timers.xpr_timing[idx].dirty.status = status;
        pers_timers.xpr_timing[idx].state = DIRTY;
        break;
    case PDC:
        pers_timers.pdc_timing[idx].dirty.status = status;
        pers_timers.pdc_timing[idx].state = DIRTY;
        break;
    }
}

void _pers_timer_update_nxt_thread(ink_time_interface_t ink_time_interface, uint8_t next_thread)
{
    // update the persistent timer dirty buffer
    pers_timers.next_info[ink_time_interface].dirty.next_thread = next_thread;
    pers_timers.next_info[ink_time_interface].state = DIRTY;
}

void _pers_timer_update_nxt_time(ink_time_interface_t ink_time_interface, uint16_t next_time)
{
    // update the persistent timer dirty buffer
    pers_timers.next_info[ink_time_interface].dirty.next_time = next_time;
    pers_timers.next_info[ink_time_interface].state = DIRTY;
}

// timer buffer is ready to commit
void _pers_timer_update_lock(ink_time_interface_t interface)
{
    switch (interface) {
    case WKUP:
        wkup_tstatus = TIMER_DIRTY;
        break;
    case XPR:
        xpr_tstatus = TIMER_DIRTY;
        break;
    case PDC:
        pdc_tstatus = TIMER_DIRTY;
        break;
    }
}

// commit into the persistent buffer

static void _commit_timer_buffers(ink_time_interface_t interface)
{
    switch (interface) {
    case WKUP:
        for (uint8_t i = 0; i < MAX_WKUP_THREADS; i++) {
            if (pers_timers.wkup_timing[i].state == DIRTY) {

                pers_timers.wkup_timing[i].persistent = pers_timers.wkup_timing[i].dirty;
                pers_timers.wkup_timing[i].state = NOT_DIRTY;
            }
        }
        break;
    case XPR:
        for (uint8_t i = 0; i < MAX_XPR_THREADS; i++) {
            if (pers_timers.xpr_timing[i].state == DIRTY) {

                pers_timers.xpr_timing[i].persistent = pers_timers.xpr_timing[i].dirty;
                pers_timers.xpr_timing[i].state = NOT_DIRTY;
            }
        }
        break;
    case PDC:
        for (uint8_t i = 0; i < MAX_PDC_THREADS; i++) {
            if (pers_timers.pdc_timing[i].state == DIRTY) {

                pers_timers.pdc_timing[i].persistent = pers_timers.pdc_timing[i].dirty;
                pers_timers.pdc_timing[i].state = NOT_DIRTY;
            }
        }
        break;
    }

    for (uint8_t i = 0; i < TIMER_TOOLS; i++) {
        if (pers_timers.next_info[interface].state == DIRTY) {
            pers_timers.next_info[interface].persistent = pers_timers.next_info[interface].dirty;
            pers_timers.next_info[interface].state = NOT_DIRTY;
        }
    }
}

void _pers_timer_commit(ink_time_interface_t interface)
{
    switch (interface) {
    case WKUP:
        if (wkup_tstatus == TIMER_DIRTY) {
            _commit_timer_buffers(interface);
            wkup_tstatus = TIMER_CLEAN;
        }
        break;
    case XPR:
        if (xpr_tstatus == TIMER_DIRTY) {
            _commit_timer_buffers(interface);
            xpr_tstatus = TIMER_CLEAN;
        }
        break;
    case PDC:
        if (pdc_tstatus == TIMER_DIRTY) {
            _commit_timer_buffers(interface);
            pdc_tstatus = TIMER_CLEAN;
        }
        break;
    }
}

timing_d_ _pers_timer_get(uint8_t idx, ink_time_interface_t interface)
{
    switch (interface) {
    case WKUP:
        return pers_timers.wkup_timing[idx].persistent;
    case XPR:
        return pers_timers.xpr_timing[idx].persistent;
    case PDC:
        return pers_timers.pdc_timing[idx].persistent;
    }
    unreachable();
}

uint16_t _pers_timer_get_data(uint8_t idx, ink_time_interface_t interface)
{
    // get the persistent timer from persistent buffer
    switch (interface) {
    case WKUP:
        return pers_timers.wkup_timing[idx].persistent.time;
    case XPR:
        return pers_timers.xpr_timing[idx].persistent.time;
    case PDC:
        return pers_timers.pdc_timing[idx].persistent.time;
    }
    unreachable();
}

uint8_t _pers_timer_get_thread_id(uint8_t idx, ink_time_interface_t interface)
{
    // get the persistent timer from persistent buffer
    switch (interface) {
    case WKUP:
        return pers_timers.wkup_timing[idx].persistent.thread_id;
    case XPR:
        return pers_timers.xpr_timing[idx].persistent.thread_id;
    case PDC:
        return pers_timers.pdc_timing[idx].persistent.thread_id;
    }
    unreachable();
}

used_st _pers_timer_get_status(uint8_t idx, ink_time_interface_t interface)
{
    // get the persistent timer from persistent buffer
    switch (interface) {
    case WKUP:
        return pers_timers.wkup_timing[idx].persistent.status;
    case XPR:
        return pers_timers.xpr_timing[idx].persistent.status;
    case PDC:
        return pers_timers.pdc_timing[idx].persistent.status;
    }
    unreachable();
}
// TODO: change types
uint8_t _pers_timer_get_nxt_thread(ink_time_interface_t ink_time_interface)
{
    // get the persistent timer from persistent buffer
    return pers_timers.next_info[ink_time_interface].persistent.next_thread;
}

uint16_t _pers_timer_get_nxt_time(ink_time_interface_t ink_time_interface)
{
    // get the persistent timer from persistent buffer
    return pers_timers.next_info[ink_time_interface].persistent.next_time;
}
