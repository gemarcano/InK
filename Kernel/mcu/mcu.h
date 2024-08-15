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
/* mcu.h
 *
 *  Created on: 15 Feb 2018
 *
 */

#ifndef MCU_H_
#define MCU_H_

#include "nv.h"

void __mcu_init(void);
void __mcu_sleep(void);

#ifndef __ti__
void __enable_interrupt(void);
void __disable_interrupt(void);
#endif

/**
 *
 * @param[in] from Address to start copy from
 * @param[out] to Address to copy to copy from
 * @param[in] size Number of words to copy
 *
 */
void __fast_word_copy(void* from, void* to, unsigned short size);

#endif /* MCU_H_ */
