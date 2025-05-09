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

#include "ink.h"
#include <mcu.h>

// indicates if this is the first boot.
// This variable is _special_. It MUST be set to zero by something else outside
// of InK.
static volatile __nv uint32_t __inited;

// global time in ticks
extern uint32_t current_ticks;

// this is the entry function for the application initialization.
// applications should implement it.
extern void __app_init();
extern void __app_reboot();

#define RANDOM_INIT_VALUE (0x00C32EAA)

int main(void)
{
    // always init microcontroller
    __mcu_init();

    // if this is the first boot
    if (__inited != RANDOM_INIT_VALUE) {
        nv_clear();
        // init the scheduler state
        __scheduler_boot_init();
        // init the event handler
        __events_boot_init();
#ifdef TIMERS_ON
        // init the timers
        __timers_boot_init();
#endif
        // init the applications
        __app_init();
        // the first and initial boot is finished
        __inited = RANDOM_INIT_VALUE;
    }
#ifdef TIMERS_ON
    else {
        __timers_init();
    }
#endif

    // will be called at each reboot of the application
    __app_reboot();

    // activate the scheduler
    __scheduler_run();

    return 0;
}
