// This file is part of InK.
//
// author = "Kasım Sinan Yıldırım "
// maintainer = "Kasım Sinan Yıldırım "
// email = "sinanyil81 [at] gmail.com"
//
// copyright = "Copyright 2018 Delft University of Technology"
// license = "LGPL"
// version = "3.0"
// status = "Production"
//
//
// InK is free software: you ca	n redistribute it and/or modify
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
// along with this program.  If not, see <http://www.gnu.org/licenses/>./*
/* isrmanager.h
 *
 *  Created on: 19 Feb 2018
 *
 */

#ifndef COMM_ISRMANAGER_H_
#define COMM_ISRMANAGER_H_

#include <scheduler/scheduler.h>

#include <stdbool.h>
#include <stdint.h>

/** Describes an event received by an ISR.
 *
 * All of the inner fields are atomic so that access to them both inside and
 * outside of an ISR remains consistent.
 */
typedef struct {
    /// A pointer to the data associated with the event. This data should be
    /// allocated in NVRAM by the caller
    _Atomic(void*) data;
    /// The size of the data in NVRAM.
    _Atomic uint32_t size;
    /// The timestamp of the event.
    _Atomic uint32_t timestamp;
} isr_event_t;

/** Initializes MAX_THREADS event data structures, and clears all popped
 * events. This must be called from a critical section.
 *
 * This must be called upon first boot.
 */
void __events_boot_init(void);

/** Tries to commit an event on the current thread, if there's an ongoing
 * event. This must be called from a critical section.
 *
 * An event is queued up for processing by calling `__event_signal_ISR`. Under
 * normal operation, this function completes the following four stages:
 *  1. Insert - The event is registered for processing. Data is inserted into
 *              structures but not finalized.
 *  2. Commit - The event is committed. Data storage is finalized and can now
 *              be accessed.
 *  3. Signal - Wakes up the thread to process the event.
 *  4. Done   - The default state, nothing is going on.
 *
 * If interrupted (by power loss), calling this function restarts on the last
 * stage not completed.
 *
 * This function should be called on reset.
 */
void __events_commit(void);

/** Checks if the thread has pending events.
 *
 * @param[in,out] thread Pointer to thread to check if it has any events
 *  pending.
 *
 * Returns True if there are pending events, false otherwise.
 */
bool __has_events(thread_t* thread);

/** Pops an event and locks it.
 *
 * FIXME how does any locking happen? What does locking even mean?
 *
 * @param[in,out] thread Pointer to thread to lock an event for.
 *
 * @returns The next event ready for procesing, if any.
 */
isr_event_t* __lock_event(thread_t* thread);

/** Releases an event after being locked.
 *
 * This commits the internal queue such that the event popped in `__lock_event`
 * is finally removed from the queue.
 *
 * @param[in,out] thread Pointer to thread to release a previously locked event
 *  for.
 */
void __release_event(thread_t* thread);

/** Checks if the event slots are full for the given thread.
 *
 * This must be called from a cristical section or an ISR
 *
 * @param[in,out] thread The thread to check its event queue.
 *
 * @returns True if the event buffer/queue is full, false otherwise.
 */
bool __event_buffer_full_ISR(thread_t* thread);

/** Queues an event up for the given thread.
 *
 * This must be called from a critical section or an ISR (usually an ISR, by
 * using the __SIGNAL_EVENT macro).
 *
 * @param[in,out] thread The thread to queue up an event for.
 * @param[in,out] event Event to queue up.
 */
void __event_signal_ISR(thread_t* thread, isr_event_t* event);

#endif /* COMM_ISRMANAGER_H_ */
