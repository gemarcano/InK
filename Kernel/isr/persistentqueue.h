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
/* persistentqueue.h
 *
 *  Created on: 19 Feb 2018
 *
 */

#ifndef COMM_PERSISTENTQUEUE_H_
#define COMM_PERSISTENTQUEUE_H_

#include "isr.h"

#include <stdbool.h>
#include <stdint.h>

#define MAX_QUEUE_ITEMS MAX_PENDING_INTERRUPTS

/** A general persistent queue structure that works via two-phase commit.
 *
 * Variables are atomic so that it is safe for an ISR or normal routines to
 * modify them.
 *
 * It is intended that this structure is stored in non-volatile memory.
 */
typedef struct {
    // Content of the queue, pointers to data. A maximum of MAX_QUEUE_ITEMS can
    // be accessed.
    _Atomic(void*) _items[MAX_QUEUE_ITEMS + 1];
    // The location of the first element in the queue.
    _Atomic uint8_t _head;
    // The location of one past the last element in the queue.
    // When _head == _tail, the queue is empty.
    _Atomic uint8_t _tail;

    // Shadow variable containing the uncommitted head
    _Atomic uint8_t __head;
    // Shadow variable containing the uncommitted head
    _Atomic uint8_t __tail;
} per_queue_t;

/** Initializes the persistent queue.
 *
 * @param[out] queue Queue to initialize.
 */
void __perqueue_init(per_queue_t* queue);

/** Returns whether the queue is empty or not.
 *
 * @param[in] queue Queue to query.
 *
 * @returns True if the queue is empty, false otherwise.
 */
bool __perqueue_is_empty(const per_queue_t* queue);

/** Returns whether the queue is full or not.
 *
 * @param[in] queue Queue to query.
 *
 * @returns True if the queue is full, false otherwise.
 */
bool __perqueue_is_full(const per_queue_t* queue);

/** Gets an element from the queue and prepares it to be removed from the
 * queue.
 *
 * The item returned, if any, is not yet removed from the queue. It is removed
 * once the operation is committed.
 *
 * @param[in,out] queue Queue to extract an element from.
 *
 * @returns A pointer to the queue element.
 */
void* __perqueue_pop(per_queue_t* queue);

/** Commits a pending queue pop.
 *
 * A pending pop operation is committed, effectively removing the element from
 * the queue.
 *
 * @param[in,out] queue Queue to commit a recent pop.
 */
void __perqueue_pop_commit(per_queue_t* queue);

/** Pushes data onto the queue.
 *
 * This operation adds the item to the queue, but it isn't registered as being
 * in the queue until it is committed.
 *
 * @param[in,out] queue Queue to push a pointer to.
 * @param[in] item Item to add to the queue.
 */
bool __perqueue_push(per_queue_t* queue, void* item);

/** Commits a pending queue push.
 *
 * A pending push operation is committed, effectively adding the element to the
 * queue.
 *
 * @param[in,out] queue Queue to commit a recent push.
 */
void __perqueue_push_commit(per_queue_t* queue);

#endif /* COMM_PERSISTENTQUEUE_H_ */
