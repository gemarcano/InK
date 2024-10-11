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
#ifndef INC_TMR_
#define INC_TMR_

// persistent timers "emulation"
// TODO:Fix that
// typedef uint16_t __pers_time_t;
//
//
#include "mcu_specifics/clk.h"

/** Initialize the timer subsystem.
 *
 * Initializes the underlying persistent timers.
 */
void __timers_init(void);

/** Commits pending updates to timers, refreshes their state, and commits that.
 */
void __reboot_timers(void);

// WKUP timers
// These timers are set to schedule an ISR for a specified time to wake up the
// MCU
/******************************************************************************/

/** Clears the status flag on wkup_d struct containing the thread/timing
 * information for the one shot timer
 */
void clear_wkup_status(uint8_t thread_id);

/** Unpacks persistent buffer from non-volatile memory to volatile memory.
 *
 * FIXME isn't this an abstraction leak? For example, this is useful on the
 * MSP430 since FRAM access is much slower than SRAM, but that's not the case
 * on the Apollo3.
 */
void unpack_wkup_to_local(void);

/** Updates the information on which thread is scheduled to execute next based
 * on timing.
 */
void refresh_wkup_timers(void);

/** Sets a one-shot timer using the wake up timer.
 *
 * @param[in] thread_id Thread to run when the timer fires.
 * @param[in] ticks Number of ticks until timer fires.
 */
void set_wkup_timer(uint8_t thread_id, uint16_t ticks);

// EXPR timers
// These timers are set to schedule a time limit in the execution of a thread
// The timer starts counting from the time initiated up to the specified amount
// of time, if the time limit has been surpassed by a death event the thread is
// evicted from the scheduler.
// The expiration counter is cleared by function call at a specified in the code.
/******************************************************************************/

/** Clears the status flag on xpr_d struct containing the thread/timing
 * information for the one shot timer
 */
void clear_xpr_status(uint8_t thread_id);

/** Unpacks persistent buffer from non-volatile memory to volatile memory.
 */
void unpack_xpr_to_local();

void refresh_xpr_timers();

void set_expire_timer(uint8_t thread_id, uint32_t ticks);

void stop_expire_timer(uint8_t thread_id);

// FIXME this gets the latest XPR thread ID processed by refresh_xpr_timers
uint8_t get_nxt_xpr(void);

// PDC timers (EXPERIMENTAL)
// These timers are set to schedule "periodic" execution of a thread
// The timer starts counting from the time initiated up to the specified amount of time,
// if the time limit has been surpassed by a death event the thread is evicted from the
// scheduler.
// The expiration counter is cleared by function call at a specified in the code.
/*************************************************************************************************************/
/** Unpacks persistent buffer from non-volatile memory to volatile memory.
 */
void unpack_pdc_to_local();

// set a periodic firing of an event
void set_periodic_timer(uint8_t thread_id, uint16_t ticks);

// stop the periodic firing of the event
void stop_periodic_timer(uint8_t thread_id);

/*Internal functions*/
void refresh_pdc_timers();

void clear_pdc_status(uint8_t thread_id);

// FIXME this gets the latest PDC thread ID processed by refresh_pdc_timers
uint8_t get_nxt_pdc(void);

#endif
