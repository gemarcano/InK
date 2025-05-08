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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/*
 * thread.c
 *
 *  Created on: 11 Feb 2018
 *
 */

#include "ink.h"

#include <stddef.h>

// runs one task inside the current thread
void __tick(thread_t* thread)
{
    switch (thread->state) {
    case TASK_READY: {
        // Make 2-phase commit temporary index point to scratch shared_data
        thread->buffer._idx = thread->buffer.idx ^ 1;

        // refresh thread shared data
        __fast_word_copy(
            thread->buffer.shared_data[thread->buffer.idx],
            thread->buffer.shared_data[thread->buffer._idx],
            thread->buffer.size);

        // get the inactive shared data
        void* buf = thread->buffer.shared_data[thread->buffer._idx];
        // Check if it is the entry task. The entry task always
        // consumes an event in the event queue.
        // We use type punning in C here for this comparison (and C only)
        if (thread->next.task == thread->entry.task) {
            // pop an event since the thread most probably woke up due to
            // an event
            isr_event_t* event = __lock_event(thread);
            // push event data to the entry task
            thread->next = (thread->entry.entry_task)(buf, event);
            // the event should be released (deleted)
            thread->state = TASK_RELEASE_EVENT;
        } else {
            thread->next = (thread->next.task)(buf);
            thread->state = TASK_FINISHED;
            break;
        }
    }
        // fallthrough
    case TASK_RELEASE_EVENT:
        // release any event which is popped by the task
        __release_event(thread);
        thread->state = TASK_FINISHED;
        // fallthrough
    case TASK_FINISHED:
        // switch stack index to commit changes
        thread->state = TASK_COMMIT;
        // fallthrough
    case TASK_COMMIT:
        // copy the real index from temporary index
        thread->buffer.idx = thread->buffer._idx;
        // Task execution finished. Check if the whole tasks are executed (thread finished)
        // We use type punning in C here for this comparison (and C only)
        if (thread->next.task == NULL) {
            __disable_interrupt();
            // check if there are any pending events
            if (!__has_events(thread)) {
                // suspend the thread if there are no pending events
                __stop_thread(thread);
            } else {
                // thread re-starts from the entry task
                thread->next = thread->entry;
                // ready to execute tasks again.
                thread->state = TASK_READY;
            }
            __enable_interrupt();
        } else {
            // ready to execute successive tasks
            thread->state = TASK_READY;
        }
        // fallthrough
    case THREAD_STOPPED:
        // FIXME Do nothing?
    }
}
