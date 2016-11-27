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

#include "io/IO_sys.h"
#include "io/IO_sys_low.h"
#include "TM4C.h"

//------------------------------------------------------------------------------
// Enable interrupts
//------------------------------------------------------------------------------
void IO_enable_interrupts()
{
  __asm__ volatile("cpsie i");
}

//------------------------------------------------------------------------------
// Disable interrupts
//------------------------------------------------------------------------------
void IO_disable_interrupts()
{
  __asm__ volatile("cpsid i");
}

//------------------------------------------------------------------------------
// Wait for an interrupt
//------------------------------------------------------------------------------
void IO_wait_for_interrupt()
{
  __asm__ volatile("wfi");
}

//------------------------------------------------------------------------------
// Set up the stack and launch the thread - implemented in assembly
//------------------------------------------------------------------------------
void TM4C_sys_start();

//------------------------------------------------------------------------------
// Start the system
//------------------------------------------------------------------------------
void IO_sys_start(uint32_t time_slice)
{
  //----------------------------------------------------------------------------
  // Set up systic and start the first thread
  //----------------------------------------------------------------------------
  time_slice *= 80;              // we have an 80MHz clock = 12.5ns ticks
  STCTRL_REG     = 0;            // turn off
  STCURRENT_REG  = 0;            // reset
  SYSPRI3_REG   |= 0xE0000000;   // priority 7
  STRELOAD_REG   = time_slice-1; // reload value
  STCTRL_REG     = 0x00000007;   // enable, core clock and interrupt arm
  TM4C_sys_start();              // set up the stack and run the thread
}

//------------------------------------------------------------------------------
// Initialize the stack
//------------------------------------------------------------------------------
void IO_sys_stack_init(IO_sys_thread *thread, void (*func)())
{
  thread->stack[IO_SYS_STACK_SIZE-1] = 0x01000000;      // PSR with thumb bit
  thread->stack[IO_SYS_STACK_SIZE-2] = (uint32_t)func;  // link register
  thread->stack_ptr = &thread->stack[IO_SYS_STACK_SIZE-16]; // top of the stack
}
