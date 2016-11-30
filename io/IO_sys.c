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
#include "IO_malloc.h"

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
// Initialize the stack
//------------------------------------------------------------------------------
void __IO_sys_stack_init(IO_sys_thread *thread, void (*func)(void *), void *arg,
  void *stack, uint32_t stack_size) {}
WEAK_ALIAS(__IO_sys_stack_init, IO_sys_stack_init);

//------------------------------------------------------------------------------
// Yield the CPU
//------------------------------------------------------------------------------
void __IO_sys_yield() {}
WEAK_ALIAS(__IO_sys_yield, IO_sys_yield);

//------------------------------------------------------------------------------
// Thread book keeping
//------------------------------------------------------------------------------
static IO_sys_thread *threads = 0;
IO_sys_thread *IO_sys_current = 0;
static IO_sys_thread iddle_thread;

//------------------------------------------------------------------------------
// Thread wrapper
//------------------------------------------------------------------------------
static void thread_wrapper(void *arg)
{
  IO_sys_thread *tcb = (IO_sys_thread *)arg;
  tcb->func();
  IO_disable_interrupts();
  if(tcb != tcb->next) {
    IO_sys_thread *cur;
    for(cur = threads; cur->next != tcb; cur = cur->next);
    cur->next = cur->next->next;
    threads = cur;
  }
  else
    threads = 0;
  IO_sys_yield();
  IO_enable_interrupts();
}

//------------------------------------------------------------------------------
// Iddle thread
//------------------------------------------------------------------------------
static void iddle_thread_func(void *arg)
{
  (void)arg;
  while(1) IO_wait_for_interrupt();
}

//------------------------------------------------------------------------------
// Register a thread
//------------------------------------------------------------------------------
int32_t IO_sys_thread_add(IO_sys_thread *thread, void (*func)(),
  uint32_t stack_size, uint8_t priority)
{
  if(stack_size < 500)
    return -IO_EINVAL;

  IO_disable_interrupts();
  thread->priority = priority;
  thread->func     = func;
  thread->sleep    = 0;
  thread->blocker  = 0;

  void *stack = IO_malloc(stack_size);
  if(stack == 0) {
    IO_enable_interrupts();
    return -IO_ENOMEM;
  }

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

  IO_sys_stack_init(thread, thread_wrapper, thread, stack, stack_size);
  IO_enable_interrupts();
  return 0;
}

//------------------------------------------------------------------------------
// Run the operating system
//------------------------------------------------------------------------------
int32_t IO_sys_run(uint32_t time_slice)
{
  IO_disable_interrupts();

  void *stack = IO_malloc(500);
  if(!stack) {
    IO_enable_interrupts();
    return -IO_ENOMEM;
  }
  IO_sys_stack_init(&iddle_thread, iddle_thread_func, 0, stack, 500);
  iddle_thread.next = &iddle_thread;

  IO_sys_current = threads;
  IO_sys_start(time_slice);
  return 0;
}

//------------------------------------------------------------------------------
// Schedule the next thread to run
//------------------------------------------------------------------------------
void IO_sys_schedule()
{
  if(!threads) {
    IO_sys_current = &iddle_thread;
    return;
  }

  IO_sys_thread *stop = IO_sys_current->next;

  if(IO_sys_current == &iddle_thread)
    stop = threads;

  IO_sys_thread *cur  = stop;
  IO_sys_thread *sel  = 0;
  int            prio = 266;

  do {
    if(!cur->sleep && !cur->blocker && cur->priority < prio) {
      sel = cur;
      prio = sel->priority;
    }
    cur = cur->next;
  }
  while(cur != stop);

  if(!sel)
    sel = &iddle_thread;

  IO_sys_current = sel;
}

//------------------------------------------------------------------------------
// Timer tick
//------------------------------------------------------------------------------
void IO_sys_timer_tick(uint64_t time)
{
  (void)time;
  IO_disable_interrupts();
  if(!threads) {
    IO_enable_interrupts();
    return;
  }
  IO_sys_thread *t = threads;
  do {
    if(t->sleep)
      --t->sleep;
    t = t->next;
  }
  while(t != threads);
  IO_enable_interrupts();
}

//------------------------------------------------------------------------------
// Sleep
//------------------------------------------------------------------------------
void IO_sys_sleep(uint32_t time)
{
  IO_sys_current->sleep = time;
  IO_sys_yield();
}

//------------------------------------------------------------------------------
// Initialize the semaphore
//------------------------------------------------------------------------------
void IO_sys_semaphore_init(IO_sys_semaphore *sem, int32_t val)
{
  if(val < 0)
    val = 0;
  *sem = val;
}

//------------------------------------------------------------------------------
// Signal
//------------------------------------------------------------------------------
void IO_sys_signal(IO_sys_semaphore *sem)
{
  IO_disable_interrupts();
  ++*sem;
  if(*sem <= 0) {
    IO_sys_thread *t;
    for(t = IO_sys_current->next; t->blocker != sem; t = t->next);
    t->blocker = 0;
  }
  IO_enable_interrupts();
}

//------------------------------------------------------------------------------
// Wait
//------------------------------------------------------------------------------
void IO_sys_wait(IO_sys_semaphore *sem)
{
  IO_disable_interrupts();
  --*sem;
  if(*sem < 0) {
    IO_sys_current->blocker = sem;
    IO_enable_interrupts();
    IO_sys_yield();
  }
  IO_enable_interrupts();
}
