// SPDX-License-Identifier: LGPL-3.0
// SPDX-FileCopyrightText: 2024 Gabriel Marcano <gmarcano@ucsd.edu>

// This file is part of InK.
//
// author = "Gabriel Marcano"
// maintainer = "Gabriel Marcano"
// email = "gmarcano@ucsd.edu"
//
// copyright = "Copyright 2024 Gabriel Marcano"
// license = "LGPL"
// version = "3.0"
// status = "Production"
//
//
// InK is free software: you ca    n redistribute it and/or modify
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
/* nv.h
 *
 *  Created on: 6 Aug 2024
 *
 */

#ifndef NV_H_
#define NV_H_

#include <stdatomic.h>

/* defines non-volatile variable */
#ifdef __GNUC__
#define __nv __attribute__((section(".nv_vars")))
#elif defined(__ti__)
#define __nv __attribute__((section(".TI.persistent")))
#else
#error "Unknown compiler, unsure where to store non-volatile memory"
#endif

typedef enum {
    STAGE_COMMIT,
    STAGE_DIRTY,
} commit_stages;

/** Clears the content of non-volatile memory used by InK.
 */
void nv_clear(void);

// All ARGS must be marked as _Atomic
// The new type _MUST_ be allocated in __nv, or a structure containing it MUST
// be in __nv
// This should be intermittent safe, but it is not thread safe.
#define DECLARE_COMMIT_DATA_TYPE(type, ARGS)                      \
    typedef struct {                                              \
        ARGS                                                      \
    } type##_data;                                                \
    typedef struct {                                              \
        _Atomic commit_stages stage;                              \
        _Atomic uint8_t valid_index;                              \
        type##_data data[2];                                      \
        type##_data committed_data;                               \
    } type;                                                       \
                                                                  \
    /** Initialize the nonvolatile container.                     \
     *                                                            \
     * @param[out] data Pointer to the container to initialize.   \
     *  This container must be in __nv memory.                    \
     *                                                            \
     * @post Container is initialized, and its memory is zero-ed. \
     */                                                           \
    void type##_init(type* data);                                 \
                                                                  \
    /** Update the data in the nonvolatile container, and marks   \
     * it dirty.                                                  \
     *                                                            \
     * @param[in,out] data Pointer to the container to update.    \
     *  This container must be initialized.                       \
     * @param[in] new_data Pointer to the contents to copy into   \
     *  the container.                                            \
     *                                                            \
     * @post The container contains the new data and is marked    \
     *  dirty, ready to be committed. Without being committed,    \
     *  any calls to type##_load will return the old data.        \
     */                                                           \
    void type##_update(type* data, const type##_data* new_data);  \
                                                                  \
    /** Commits pending data in the container.                    \
     *                                                            \
     * @param[in,out] data Container to commit data into. The     \
     *  container must be initialized.                            \
     *                                                            \
     * @returns True if there was data to commit and it was       \
     *  committed, false otherwise. After being committed, the    \
     *  new data will be returned from subsequent type##_load     \
     *  calls.                                                    \
     */                                                           \
    bool type##_commit(type* data);                               \
                                                                  \
    /** Returns the committed data in the container.              \
     *                                                            \
     * @param[in] data The container to retrieve committed data   \
     *  from. The container must be initialized.                  \
     *                                                            \
     * @returns A copy of the committed data in the container.    \
     */                                                           \
    type##_data type##_load(type* data);                          \
                                                                  \
    /** Update and commit data into the container at the same     \
     *  time.                                                     \
     *                                                            \
     * @param[in,out] data Container to update and commit data    \
     *  into. The container must be initialized.                  \
     * @param[in] new_data Data to store in the container.        \
     *                                                            \
     * @post The new data is committed into the container.        \
     */                                                           \
    void type##_store(type* data, const type##_data* new_data);

#define DEFINE_COMMIT_DATA_TYPE(type)                           \
    void type##_init(type* data)                                \
    {                                                           \
        data->stage = STAGE_COMMIT;                             \
        data->data[0] = (type##_data) { 0 };                    \
        data->data[1] = (type##_data) { 0 };                    \
        data->valid_index = 0;                                  \
    }                                                           \
    void type##_update(type* data, const type##_data* new_data) \
    {                                                           \
        data->data[data->valid_index ^ 1] = *new_data;          \
        data->stage = STAGE_DIRTY;                              \
    }                                                           \
    bool type##_commit(type* data)                              \
    {                                                           \
        if (data->stage == STAGE_DIRTY) {                       \
            atomic_fetch_xor(&data->valid_index, 1);            \
            data->stage = STAGE_COMMIT;                         \
            return true;                                        \
        }                                                       \
        return false;                                           \
    }                                                           \
    type##_data type##_load(type* data)                         \
    {                                                           \
        return data->data[data->valid_index];                   \
    }                                                           \
    void type##_store(type* data, const type##_data* new_data)  \
    {                                                           \
        type##_update(data, new_data);                          \
        type##_commit(data);                                    \
    }

#define DEFINE_COMMIT_DATA(type, name) \
    __nv type name

#endif /* NV_H_ */
