// This file is part of InK.
//
// author = "dpatoukas "
// maintainer = "dpatoukas "
// email = "dpatoukas@gmail.com"
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

// This application samples a microphone via ADC
// ADC pins and hardware must be configured according to the used device
//
//
// If no REAL_MIC is provided a virtual sample will be used
#include "app.h"
#include "ink.h"

#include <stddef.h>

__shared(
    uint16_t counter;)

    // Define two tasks
    ENTRY_TASK(task1);
TASK(task2);

void thread1_init()
{
    // create a thread with priority " " and entry task task1
    __CREATE(THREAD1, task1);
    __SIGNAL(THREAD1);
}

void __app_reboot()
{
}

uint16_t counter;
ENTRY_TASK(task1)
{
    counter++;
    // Returns next task to run
    return (task_t)task2;
}

TASK(task2)
{

    // This re-queues THREAD1 to re-run
    __SIGNAL(THREAD1);
    return NULL;
}
