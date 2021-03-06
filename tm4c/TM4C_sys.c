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

  //----------------------------------------------------------------------------
  // Yield
  //----------------------------------------------------------------------------
  IO_sys_yield();
  IO_enable_interrupts();
}

//------------------------------------------------------------------------------
// Initialize the stack
//------------------------------------------------------------------------------
void IO_sys_stack_init(IO_sys_thread *thread, void (*func)(void *), void *arg,
  void *stack, uint32_t stack_size)
{
  uint32_t sp1 = (uint32_t)stack;
  uint32_t sp2 = (uint32_t)stack;
  sp2 += stack_size;
  sp2 = (sp2 >> 3) << 3;          // the stack base needs to be 8-aligned
  if(sp1 % 4)
    sp1 = ((sp1 >> 2) << 2) + 4;  // make the end of the stack 4-aligned
  stack_size = (sp2 - sp1) / 4;   // new size in double words

  uint32_t *sp = (uint32_t *)sp1;
  sp[stack_size-1] = 0x01000000;          // PSR with thumb bit
  sp[stack_size-2] = (uint32_t)func;      // program counter
  sp[stack_size-3] = 0xffffffff;          // link register
  sp[stack_size-8] = (uint32_t)arg;       // r0 - the argument
  thread->stack_ptr = &sp[stack_size-16]; // top of the stack
}

//------------------------------------------------------------------------------
// Yield the CPU
//------------------------------------------------------------------------------
void IO_sys_yield()
{
  STCURRENT_REG = 0;          // clear the systick counter
  INTCTRL_REG   = 0x04000000; // trigger systick
}
