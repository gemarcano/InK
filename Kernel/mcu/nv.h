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

#endif /* NV_H_ */
