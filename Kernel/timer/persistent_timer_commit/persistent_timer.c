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

DEFINE_COMMIT_DATA_TYPE(timing_d)
DEFINE_COMMIT_DATA_TYPE(next_d)
DEFINE_COMMIT_DATA_TYPE(pers_time_d)

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

void _pers_timer_boot_init(void)
{
    for (uint8_t i = 0; i < MAX_TIMED_THREADS; i++) {
        timing_d_init(&pers_timers.wkup_timing[i]);
        timing_d_init(&pers_timers.xpr_timing[i]);
        timing_d_init(&pers_timers.pdc_timing[i]);
        pers_timers.wkup_timing[i].committed_data.status = NOT_USED;
        pers_timers.xpr_timing[i].committed_data.status = NOT_USED;
        pers_timers.pdc_timing[i].committed_data.status = NOT_USED;
    }
}

void _pers_timer_update_data(uint8_t idx, ink_time_interface_t interface, const timing_d_data* data)
{
    // update the persistent timer dirty buffer
    switch (interface) {
    case WKUP:
        timing_d_update(&pers_timers.wkup_timing[idx], data);
        break;
    case XPR:
        timing_d_update(&pers_timers.xpr_timing[idx], data);
        break;
    case PDC:
        timing_d_update(&pers_timers.pdc_timing[idx], data);
        break;
    }
}

void _pers_timer_update_nxt_thread(ink_time_interface_t ink_time_interface, const next_d_data* data)
{
    // update the persistent timer dirty buffer
    next_d_update(&pers_timers.next_info[ink_time_interface], data);
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
            timing_d_commit(&pers_timers.wkup_timing[i]);
        }
        break;
    case XPR:
        for (uint8_t i = 0; i < MAX_XPR_THREADS; i++) {
            timing_d_commit(&pers_timers.xpr_timing[i]);
        }
        break;
    case PDC:
        for (uint8_t i = 0; i < MAX_PDC_THREADS; i++) {
            timing_d_commit(&pers_timers.pdc_timing[i]);
        }
        break;
    }

    for (uint8_t i = 0; i < TIMER_TOOLS; i++) {
        next_d_commit(&pers_timers.next_info[i]);
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

timing_d_data _pers_timer_get(uint8_t idx, ink_time_interface_t interface)
{
    switch (interface) {
    case WKUP:
        return timing_d_load(&pers_timers.wkup_timing[idx]);
    case XPR:
        return timing_d_load(&pers_timers.xpr_timing[idx]);
    case PDC:
        return timing_d_load(&pers_timers.pdc_timing[idx]);
    }
    unreachable();
}

uint16_t _pers_timer_get_data(uint8_t idx, ink_time_interface_t interface)
{
    return _pers_timer_get(idx, interface).time;
}

uint8_t _pers_timer_get_thread_id(uint8_t idx, ink_time_interface_t interface)
{
    return _pers_timer_get(idx, interface).thread_id;
}

used_st _pers_timer_get_status(uint8_t idx, ink_time_interface_t interface)
{
    return _pers_timer_get(idx, interface).status;
}

// TODO: change types
uint8_t _pers_timer_get_nxt_thread(ink_time_interface_t ink_time_interface)
{
    // get the persistent timer from persistent buffer
    return next_d_load(&pers_timers.next_info[ink_time_interface]).next_thread;
}

uint16_t _pers_timer_get_nxt_time(ink_time_interface_t ink_time_interface)
{
    // get the persistent timer from persistent buffer
    return next_d_load(&pers_timers.next_info[ink_time_interface]).next_time;
}
