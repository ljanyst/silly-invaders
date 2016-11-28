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

#define OFF_STACK_PTR 0
#define OFF_FLAGS     4
#define FLAG_FPU      0x01

  .thumb
  .syntax unified

  .global IO_sys_current
  .global IO_sys_schedule

  .text

//------------------------------------------------------------------------------
// Store the state of the current thread, call the scheduler, and start the new
// thread
//------------------------------------------------------------------------------
  .global systick_handler
  .type systick_handler STT_FUNC
  .thumb_func
  .align  2
systick_handler:
  cpsid i                     // disable interrupts
  push  {r4-r11}              // push r4-11
  ldr   r0, =IO_sys_current   // pointer to IO_sys_current to r1
  ldr   r1, [r0]              // r1 = OS_current

  ubfx  r2, lr, #4, #1        // extract the fourth bit from the lr register
  cbnz  r2, .Lsave_stack      // no FPU context to save
  vstmdb sp!, {s16-s31}       // push FPU registers, this triggers pushing of
                              // s0-s15
  ldr   r2, [r1, #OFF_FLAGS]  // load the flags
  orr   r2, r2, #FLAG_FPU     // set the FPU context flag
  str   r2, [r1, #OFF_FLAGS]  // store the flags

.Lsave_stack:
  str   sp, [r1, #OFF_STACK_PTR] // store the stack pointer at *OS_current

  push  {r0, lr}              // calling c code, so store r0 and the link
                              // register
  bl    IO_sys_schedule       // call the scheduler
  pop   {r0, lr}              // restore r0 and lr

  ldr   r1, [r0]              // load the new TCB pointer to r1
  ldr   sp, [r1, #OFF_STACK_PTR] // get the stack pointer of the new thread

  orr   lr, lr, #0x10         // clear the floating point flag in EXC_RETURN
  ldr   r2, [r1, #OFF_FLAGS]  // load the flags
  tst   r2, #0x01             // see if we have the FPU context
  beq   .Lrestore_regs        // no FPU context
  vldmia sp!, {s16-s31}       // pop the FPU registers
  bic   lr, lr, #0x10         // set the floating point flag in EXC_RETURN

.Lrestore_regs:
  pop   {r4-r11}              // restore regs r4-11
  cpsie i                     // enable interrupts
  bx    lr                    // exit the interrupt, restore r0-r3, r12, lr, pc,
                              // psr

//------------------------------------------------------------------------------
// Start the first thread
//------------------------------------------------------------------------------
  .global TM4C_sys_start
  .type   TM4C_sys_start STT_FUNC
  .thumb_func
  .align  2
TM4C_sys_start:
  ldr   r0, =IO_sys_current   // pointer to IO_sys_current to r0
  ldr   r1, [r0]              // pointer to the first TCB to r1
  ldr   sp, [r1]              // get the stack pointer
  add   sp, sp, #32           // discard the initial registers
  pop   {r0}                  // pass the thread func parameter
  add   sp, sp, #20           // discard the initial registers
  pop   {lr}                  // start location
  add   sp, sp, #4            // discard psr
  cpsie i                     // enable interrupts
  bx    lr                    // start first thread
