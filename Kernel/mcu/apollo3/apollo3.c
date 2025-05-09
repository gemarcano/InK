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

/* apollo3.c
 *
 *  Created on: 6 Aug 2024
 *
 */

#include <isr/isr.h>

#include <asimple/systick.h>

#include "am_bsp.h"
#include "am_hal_pwrctrl.h"
#include "am_mcu_apollo.h"
#include "am_util.h"

#include <stdint.h>
#include <string.h>

void __mcu_init(void)
{
    // FIXME initialize apollo3
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();
    am_bsp_low_power_init();
    am_hal_sysctrl_fpu_enable();
    am_hal_sysctrl_fpu_stacking_enable(true);

    // After basic init is done, enable interrupts
    am_hal_interrupt_master_enable();

    // initialize systick
    systick_reset();
    systick_start();

    // FIXME register GPIO interrupt for triggering sleep
}

void Reset_Handler(void); // This must be defined by the application, as we jump to
                          // it to re-initialize ink upon waking up.

void __mcu_sleep(void)
{
    // FIXME is this OK? For small programs it is, but is this OK of an assumption?
    // Explicitly enable power to only the bottom half of flash
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_FLASH_512K);

    // Request that only 64KB of SRAM is retained
    am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_ALL);
    am_hal_pwrctrl_memory_deepsleep_retain(AM_HAL_PWRCTRL_MEM_SRAM_64K_DTCM);

    uint32_t DEVPWREN = PWRCTRL->DEVPWREN;
    // Power everything down
    PWRCTRL->DEVPWREN = 0;

    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

    // Restore peripheral power
    PWRCTRL->DEVPWREN = DEVPWREN;

    Reset_Handler();
}

void __enable_interrupt(void)
{
    am_hal_interrupt_master_enable();
}

void __disable_interrupt(void)
{
    am_hal_interrupt_master_disable();
}

void __fast_word_copy(const void* from, void* to, size_t size)
{
    // Apollo3 doesn't have general purposes DMA, so just use memcpy
    memcpy(to, from, size);
}

void enter_critical_section(critical_section* section)
{
    // copy current interrupt state
    section->state = __get_PRIMASK();
    __set_PRIMASK(1);
}

void exit_critical_section(critical_section* section)
{
    __set_PRIMASK(section->state);
    section->state = 0;
}

void nv_clear(void)
{
    extern unsigned char* _nv;
    extern unsigned char* _nv_end;
    memset(_nv, 0, _nv_end - _nv);
}
