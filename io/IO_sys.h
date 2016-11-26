//------------------------------------------------------------------------------
// Copyright (c) 2016 by Lukasz Janyst <lukasz@jany.st>
//------------------------------------------------------------------------------
// This file is part of silly-invaders.
//
// silly-invaders is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// silly-invaders is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with silly-invaders.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------------

#pragma once

#include <stdint.h>

//------------------------------------------------------------------------------
//! Enable interrupts
//------------------------------------------------------------------------------
void IO_enable_interrupts();

//------------------------------------------------------------------------------
//! Disable interrupts
//------------------------------------------------------------------------------
void IO_disable_interrupts();

//------------------------------------------------------------------------------
//! Wait for an interrupt
//------------------------------------------------------------------------------
void IO_wait_for_interrupt();

//------------------------------------------------------------------------------
//! Thread control block
//------------------------------------------------------------------------------
#define IO_SYS_STACK_SIZE 250

struct IO_sys_thread {
  uint32_t *stack_ptr;
  struct IO_sys_thread *next;
  uint32_t  stack[IO_SYS_STACK_SIZE] __attribute__((aligned(8)));
  uint8_t   priority;
  uint8_t   profiler;
};

typedef struct IO_sys_thread IO_sys_thread;

//------------------------------------------------------------------------------
//! Register a thread
//!
//! @param thread   thread control block
//! @param func     thread function
//! @param priority thread priority
//------------------------------------------------------------------------------
void IO_sys_thread_add(IO_sys_thread *thread, void (*func)(), uint8_t priority);

//------------------------------------------------------------------------------
//! Run the operating system
//!
//! @param time_slice timeslice in microseconds
//------------------------------------------------------------------------------
void IO_sys_run(uint32_t time_slice);
