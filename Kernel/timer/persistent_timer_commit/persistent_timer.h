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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.//perssistent timers data
#ifndef PERS_TMR_
#define PERS_TMR_

#include "ink.h"

// how many timing Ink interfaces are used
#define TIMER_TOOLS 3
#define MAX_TIMED_THREADS 3
#define MAX_WKUP_THREADS 3
#define MAX_XPR_THREADS 3
#define MAX_PDC_THREADS 1

/// Represents whether a structure contains dirty data or not.
typedef enum {
    NOT_DIRTY,
    DIRTY
} dirty_st;

/// Represents whether a timer is in use or not.
typedef enum {
    NOT_USED,
    USED
} used_st;

/// Represents the InK timer type.
typedef enum {
    WKUP,
    PDC,
    XPR
} ink_time_interface_t;

/** Contains the timing data for a timer.
 */
typedef struct
{
    /// Indicates whether this timer is being used or not.
    used_st status;
    /// The unique thread ID that's using this timer.
    uint8_t thread_id;

    // FIXME why is this signed?
    int32_t time; /** remaining time for thread execution*/
} timing_d_;

/** Holds both persistent and dirty timer structures.
 */
typedef struct
{
    timing_d_ persistent;
    timing_d_ dirty;
    /// Indicates whether or not there's pending data that needs to be
    /// committed.
    dirty_st state;
} timing_d;

/** Contains the next thread to be executed persistent timer
 */
typedef struct
{
    /// Whether the structure is in use or not.
    used_st status;
    /// The ID of the next thread candidate.
    uint8_t next_thread;
    /// The time the next thread candidate should run at (FIXME is this true?)
    uint16_t next_time;
} next_d_;

/** Holds both persistent and dirty next thread structures.
 */
typedef struct
{
    next_d_ persistent;
    next_d_ dirty;
    dirty_st state;
} next_d;

/** Contains system on/off time.
 */
typedef struct
{
    /// The amount of time the system has been on (FIXME units?)
    uint16_t on_time;
    /// The amount of time the system has been off (FIXME units?)
    uint16_t off_time;
} pers_time_d_;

/** Holds both persistent and dirty system time structures.
 */
typedef struct {
    pers_time_d_ persistent;
    pers_time_d_ dirty;
    dirty_st state;
} pers_time_d;

/** Contains timing data for WakeUp/Expiration/Periodic timer, global time, and
 * next thread to be fired by each timer.
 */
typedef struct
{
    /// Timing data for the wake-up timers.
    timing_d wkup_timing[MAX_WKUP_THREADS];
    /// Timing data for the expiration timers.
    timing_d xpr_timing[MAX_XPR_THREADS];
    /// Timing data for the periodic timers.
    timing_d pdc_timing[MAX_PDC_THREADS];
    /// Data on FIXME what are timer tools?
    next_d next_info[TIMER_TOOLS];
    /// Information about the global time on and off.
    pers_time_d time_bank;
} pers_timers_t;

/** Initialize the persistent timers internal structures.
 *
 * This sets all of the structures ready for use, as in, marked as not in use.
 * FIXME should anything else be cleared? Should this be called on all boots or
 * just first boot?
 */
void _pers_timer_init(void);

/** Updates the time value of the specified timer.
 *
 * This sets the timer's dirty flag to true.
 *
 * @param idx The index of the timer to update.
 * @param interface The type of the timer to update.
 * @param time_data The data to store in the timer.
 */
void _pers_timer_update_data(uint8_t idx, ink_time_interface_t interface, uint32_t time_data);

/** Updates the thread value of the specified timer.
 *
 * This sets the timer's dirty flag to true.
 *
 * @param idx The index of the timer to update.
 * @param interface The type of the timer to update.
 * @param thread_id Thread ID to store in the timer.
 */
void _pers_timer_update_thread_id(uint8_t idx, ink_time_interface_t interface, uint8_t thread_id);

/** Updates the status value of the specified timer.
 *
 * This sets the timer's dirty flag to true.
 *
 * @param idx The index of the timer to update.
 * @param interface The type of the timer to update.
 * @param status Status to store in the timer.
 */
void _pers_timer_update_status(uint8_t idx, ink_time_interface_t interface, used_st status);

/** Updates the next thread to run for the specified timer.
 *
 * This sets the next thread timer's dirty flag to true.
 *
 * @param interface The type of the timer to associate a next thread with.
 * @param next_tread The ID of the next thread for executing set for this timer
 */
void _pers_timer_update_nxt_thread(ink_time_interface_t interface, uint8_t next_thread);

/** Updates the time remaining in the timer for the next event.
 *
 * @param interface The type of the timer to update its next trigger time.
 * @param next_time The remaining time for next interrupt event (FIXME what units?)
 */
void _pers_timer_update_nxt_time(ink_time_interface_t interface, uint16_t next_time);

// collect data from the perssistent buffer
/** Gets the timing structure for the specified timer.
 *
 * @param idx The index of the timer to fetch.
 * @param interface The type of the timer to fetch.
 *
 * @return Timing information from the persistent buffer.
 */
timing_d_ _pers_timer_get(uint8_t idx, ink_time_interface_t interface);

/** Get the timer data for the specified timer.
 *
 * This is a subset of the data returned by _pers_timer_get.
 *
 * @param idx The index of the timer to fetch.
 * @param interface  The type of the timer to fetch.
 *
 * @return The time (FIXME units?) of the specified timer.
 */
uint16_t _pers_timer_get_data(uint8_t idx, ink_time_interface_t interface);

/** Gets the thread id for the specified timer.
 *
 * This is a subset of the data returned by _pers_timer_get.
 *
 * @param idx The index of the timer to fetch.
 * @param interface The type of the timer to fetch.
 *
 * @return The thread id of the specified timer.
 */
uint8_t _pers_timer_get_thread_id(uint8_t idx, ink_time_interface_t interface);

/** Gets the status for the specified timer.
 *
 * This is a subset of the data returned by _pers_timer_get.
 *
 * @param idx The index of the timer to fetch.
 * @param interface The type of the timer to fetch.
 *
 * @return The status of the specified timer.
 */
used_st _pers_timer_get_status(uint8_t idx, ink_time_interface_t interface);

/** Returns the next thread id of the specified timer.
 *
 * @param idx The index of the timer to fetch.
 * @param interface The type of the timer to fetch.
 *
 * @return The thread ID of the next thread.
 */
uint8_t _pers_timer_get_nxt_thread(ink_time_interface_t interface);

/** Returns the next time of the specified timer.
 *
 * @param idx The index of the timer to fetch.
 * @param interface The type of the timer to fetch.
 *
 * @return The next time of the timer.
 */
uint16_t _pers_timer_get_nxt_time(ink_time_interface_t interface);

/** Marks the specified timer as ready to be committed to non-volatile memory.
 *
 * FIXME This needs to be safe for ISR use
 * FIXME nothing about this implementation actually uses the underlying variable as a lock...
 *
 * @param interface The type of the timer to mark as ready for commit.
 */
void _pers_timer_update_lock(ink_time_interface_t interface);

/** Commits the memory of the specified timer to non-volatile memory.
 *
 * FIXME Thids needs to be safe for ISR use
 * This only does something if the timer has been previously locked.
 *
 * @param interface The type of the timer to commit.
 */
void _pers_timer_commit(ink_time_interface_t interface);

#endif
