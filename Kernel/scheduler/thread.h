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
 * thread.h
 *
 *  Created on: 12 Feb 2018
 *
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <stddef.h>
#include <stdint.h>

// the state of the threads
typedef enum { TASK_READY = 1,
    TASK_RELEASE_EVENT = 2,
    TASK_FINISHED = 4,
    TASK_COMMIT = 8,
    THREAD_STOPPED = 16 } state_t;

// each thread will hold the double buffer for the variables
// shared by the tasks it is encapsulating.
typedef struct {
    // holds pointers to the active and inactive shared non-volatile data
    // The actual type is dependent on the __shared data used by the thread
    // owning this buffer.
    void* shared_data[2];
    // Index of the currently valid shared data
    _Atomic uint8_t idx;
    // Temporary index, used for 2-phase commit
    _Atomic uint8_t _idx;
    // Size of the data pointed to by each shared_data index
    size_t size;
} buffer_t;

// type forward declaration, there's a circular dependency between thread.h and
// isrmanager.h
typedef struct isr_event_t_ isr_event_t;

// type forward declarate for non_volatile_data, this type must be defined by
// an application through the __shared macro.
typedef struct non_volatile_data_ non_volatile_data_t;

/** Struct holding a pointer to a task function.
 *
 * This is primarily used because it's not possible to have a function that
 * returns itself, but it can be worked around through the use of a struct.
 * Additionally, this way we can encode both the entry task and other tasks
 * through a union and have them fall under the same type.
 */
struct task_t {
    union {
        /** Pointer to InK task function.
         *
         * @param[in,out] buffer A pointer to the non-volatile storage used by
         *  the application.
         *
         * @returns The next task to execute.
         */
        struct task_t (*task)(non_volatile_data_t* buffer);
        /** Pointer to InK entry function.
         *
         * @param[in,out] buffer A pointer to the non-volatile storage used by
         *  the application.
         * @param[in,out] event A pointer to an event (that hopefully woke up
         *  this thread). This could be NULL if there is no event (e.g. we're
         *  waking up for the first time).
         */
        struct task_t (*entry_task)(non_volatile_data_t* buffer, isr_event_t* event);
    };
};

// the main thread structure that holds all necessary info
// to execute the computation represented by the wired
// tasks
typedef struct {
    uint8_t priority; // thread priority (unique)
    _Atomic state_t state; // thread state
    struct task_t entry; // the first task to be executed
    struct task_t next; // the current task to be executed
    buffer_t buffer; // holds task shared persistent variables
    uint16_t sing_timer; // holds the time when the thread will be executed
    uint16_t pdc_timer; // holds the time for "periodic" execution of the thread
    uint16_t expr_timer; // hold the expiration time of the thread from time of completion
    uint16_t pdc_period; // holds the current period
} thread_t;

// allocates a double buffer for the persistent variables in non-volatile
// memory. There can only be one of these per translation unit. Also, The data
// is not shared across translation units, even if this is called in a header.
#define __shared(first, ...)        \
    struct non_volatile_data_ {     \
        _Alignas(sizeof(int)) first \
            __VA_ARGS__             \
    };                              \
    static __nv non_volatile_data_t __persistent_vars[2]

// runs one task inside the current thread.
void __tick(thread_t* thread);

#endif /* THREAD_H_ */
