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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#ifndef _CLK_H_
#define _CLK_H_
#include "ink.h"

/** Starts the __get_time interface.
 *
 * This runs any initialization code required to make __get_time() function.
 *
 * As an example, for the MSP430 implementation, this initializes the RTC and
 * gets a timestamp from it to ensure the continuity of the internal time since
 * boot, and starts the __get_time() dedicated timer.
 */
void __get_time_init(void);

/** Stops the __get_time interface.
 *
 * This stops any timers __get_time requires.
 */
void __get_time_stop(void);

/* Returns the time since first boot in milliseconds.
 *
 * FIXME at uint32_t, this overflows after 49 days... is that OK?
 */
uint32_t __get_time(void);

/** Sets up the RTC.
 *
 * Turns on any required peripherals to communicate with the RTC.
 */
void __setup_rtc(void);

/** Sets up system clock.
 *
 * Sets up system clock frequency, and initializes anything needed to use the
 * timers.
 */
void __setup_clock(void);

/** Sets the Wakeup timer to fire in the given number of ticks.
 *
 * @param[in] ticks Number of ticks until timer fires.
 */
void set_timer_wkup(uint16_t ticks);

/** Sets the Expiration timer to fire in the given number of ticks.
 *
 * @param[in] ticks Number of ticks until timer fires.
 */
void set_timer_xpr(uint16_t ticks);

/** Sets the Periodic timer to fire in the given number of ticks.
 *
 * @param[in] ticks Number of ticks until timer fires.
 */
void set_timer_pdc(uint16_t ticks);

/** Stops the Expiration timer.
 *
 * This resets the internal counter.
 */
void stop_timer_xpr(void);

/** Stops the Wakeup timer.
 *
 * This resets the internal counter.
 */
void stop_timer_wkup(void);

/** Stops the Periodic timer.
 *
 * This resets the internal counter.
 */
void stop_timer_pdc(void);

#endif
