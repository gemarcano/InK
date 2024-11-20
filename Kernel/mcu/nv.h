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
/* nv.h
 *
 *  Created on: 6 Aug 2024
 *
 */

#ifndef NV_H_
#define NV_H_

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

// All ARGS must be marked as _Atomic
// The new type _MUST_ be allocated in __nv, or a structure containing it MUST
// be in __nv
#define DECLARE_COMMIT_DATA_TYPE(type, ARGS)                     \
    typedef struct {                                             \
        ARGS                                                     \
    } type##_data;                                               \
    typedef struct {                                             \
        _Atomic commit_stages stage;                             \
        type##_data update_data;                                 \
        type##_data committed_data;                              \
    } type;                                                      \
    void type##_init(type* data);                                \
    void type##_update(type* data, const type##_data* new_data); \
    bool type##_commit(type* data);                              \
    bool type##_get_valid(type* data, type##_data* result);

#define DEFINE_COMMIT_DATA_TYPE(type)                           \
    void type##_init(type* data)                                \
    {                                                           \
        data->stage = STAGE_COMMIT;                             \
        data->update_data = (type##_data) { 0 };                \
        data->committed_data = (type##_data) { 0 };             \
    }                                                           \
    void type##_update(type* data, const type##_data* new_data) \
    {                                                           \
        data->update_data = *new_data;                          \
        data->stage = STAGE_DIRTY;                              \
    }                                                           \
    bool type##_commit(type* data)                              \
    {                                                           \
        if (data->stage == STAGE_DIRTY) {                       \
            data->committed_data = data->update_data;           \
            data->stage = STAGE_COMMIT;                         \
            return true;                                        \
        }                                                       \
        return false;                                           \
    }                                                           \
    bool type##_get_valid(type* data, type##_data* result)      \
    {                                                           \
        if (data->stage == STAGE_COMMIT) {                      \
            *result = data->committed_data;                     \
            return true;                                        \
        }                                                       \
        return false;                                           \
    }

#define DEFINE_COMMIT_DATA(type, name) \
    __nv type name

#endif /* NV_H_ */
