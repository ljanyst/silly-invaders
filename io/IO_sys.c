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

#define __IO_IMPL__
#include "IO_sys.h"
#include "IO_sys_low.h"
#include "IO_utils.h"
#include "IO_error.h"

//------------------------------------------------------------------------------
// Enable interrupts
//------------------------------------------------------------------------------
void __IO_enable_interrupts() {}
WEAK_ALIAS(__IO_enable_interrupts, IO_enable_interrupts);

//------------------------------------------------------------------------------
// Disable interrupts
//------------------------------------------------------------------------------
void __IO_disable_interrupts() {}
WEAK_ALIAS(__IO_disable_interrupts, IO_disable_interrupts);

//------------------------------------------------------------------------------
// Wait for an interrupt
//------------------------------------------------------------------------------
void __IO_wait_for_interrupt() {}
WEAK_ALIAS(__IO_wait_for_interrupt, IO_wait_for_interrupt);

//------------------------------------------------------------------------------
// Initialize systick
//------------------------------------------------------------------------------
void __IO_sys_tick_init(uint32_t timeslice) {}
WEAK_ALIAS(__IO_sys_tick_init, IO_sys_tick_init);

//------------------------------------------------------------------------------
// Start the system
//------------------------------------------------------------------------------
void __IO_sys_start(uint32_t time_slice) {}
WEAK_ALIAS(__IO_sys_start, IO_sys_start);

//------------------------------------------------------------------------------
// Thread book keeping
//------------------------------------------------------------------------------
static IO_sys_thread *threads = 0;
IO_sys_thread *IO_sys_current = 0;

//------------------------------------------------------------------------------
// Register a thread
//------------------------------------------------------------------------------
void IO_sys_thread_add(IO_sys_thread *thread, void (*func)(), uint8_t priority)
{
  IO_disable_interrupts();
  thread->priority = priority;
  if(threads == 0) {
    threads = thread;
    thread->next = thread;
  }
  else {
    IO_sys_thread *last;
    for(last = threads; last->next != threads; last = last->next);
    thread->next = threads;
    threads = thread;
    last->next = threads;
  }

  thread->stack[IO_SYS_STACK_SIZE-1] = 0x01000000;      // PSR with thumb bit
  thread->stack[IO_SYS_STACK_SIZE-2] = (uint32_t)func;  // link register
  thread->stack_ptr = &thread->stack[IO_SYS_STACK_SIZE-16]; // top of the stack
  IO_enable_interrupts();
}

//------------------------------------------------------------------------------
// Run the operating system
//------------------------------------------------------------------------------
void IO_sys_run(uint32_t time_slice)
{
  IO_disable_interrupts();
  IO_sys_current = threads;
  IO_sys_start(time_slice);
}


//------------------------------------------------------------------------------
// Schedule the next thread to run
//------------------------------------------------------------------------------
void IO_sys_schedule()
{
  IO_sys_current = IO_sys_current->next;
}
