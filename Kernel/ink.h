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
 * ink.h
 *
 *  Created on: 14 Feb 2018
 *
 */

#ifndef INK_H_
#define INK_H_

#include "channel/channel.h"
#include "isr/isr.h"
#include "mcu/mcu.h"
#include "scheduler/scheduler.h"
#include "timer/timer.h"

/*
 * Module settings
 */

// Define the WKUP_TIMER macro to enable wake-up timer support in InK.
#define WKUP_TIMER
// Define the XPR_TIMER macro to enable expiration timer support in InK.
#define XPR_TIMER
// Define the TIMERS_ON macro to timekeeping support in InK.
#define TIMERS_ON

#endif /* INK_H_ */
