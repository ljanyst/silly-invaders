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

#include <io/IO.h>
#include <io/IO_error.h>
#include "TM4C.h"
#include "TM4C_gpio.h"
#include "TM4C_timer.h"

//------------------------------------------------------------------------------
// Port interrupts
//------------------------------------------------------------------------------
static const int timer_interrupt[12] = {
  19, 21, 23, 35, 70, 92, 94, 96, 98, 100, 102, 104};

static struct IO_io *timer_devices[12];

//------------------------------------------------------------------------------
// Handle interrupts
//------------------------------------------------------------------------------
static void timer_handler(uint8_t module)
{
  int module_offset = module * GPTM_MODULE_OFFSET;
  if(timer_devices[module] && timer_devices[module]->event)
    timer_devices[module]->event(timer_devices[module], IO_EVENT_TICK);
  GPTM_REG(module_offset, GPTM_ICR) |= 1; // ack the interrupt
}

void timer0a_32_handler() { timer_handler(0); }
void timer1a_32_handler() { timer_handler(1); }
void timer2a_32_handler() { timer_handler(2); }
void timer3a_32_handler() { timer_handler(3); }
void timer4a_32_handler() { timer_handler(4); }
void timer5a_32_handler() { timer_handler(5); }
void timer0a_64_handler() { timer_handler(6); }
void timer1a_64_handler() { timer_handler(7); }
void timer2a_64_handler() { timer_handler(8); }
void timer3a_64_handler() { timer_handler(9); }
void timer4a_64_handler() { timer_handler(10); }
void timer5a_64_handler() { timer_handler(11); }

//------------------------------------------------------------------------------
// Convert nanoseconds to the clock ticks, in our case 12.5 ns/tick (80MHz)
//------------------------------------------------------------------------------
static uint64_t nsecs2ticks(uint64_t ns)
{
  ns <<= 1;
  return  ns/25;
}

//------------------------------------------------------------------------------
// Convert the clock ticks to nanoseconds, in our case 12.5 ns/tick (80MHz)
//------------------------------------------------------------------------------
static uint64_t ticks2nsecs(uint64_t ticks)
{
  ticks *= 25;
  return  ticks >> 1;
}

//------------------------------------------------------------------------------
// Set 32-bit timer state
//------------------------------------------------------------------------------
static int32_t timer32_write(IO_io *io, const void *data, uint32_t length)
{
  if(length != 1)
    return -IO_EINVAL;

  uint64_t val = nsecs2ticks(*(const uint64_t*)data);
  if(val > 0xffffffff)
    return -IO_EINVAL;

  int module_offset = io->channel * GPTM_MODULE_OFFSET;
  GPTM_REG(module_offset, GPTM_TAILR) = (uint32_t)val;

  // enable the timer
  GPTM_REG(module_offset, GPTM_CTL) |= 1;
  return 1;
}

//------------------------------------------------------------------------------
// Set 64-bit timer state
//------------------------------------------------------------------------------
static int32_t timer64_write(IO_io *io, const void *data, uint32_t length)
{
  if(length != 1)
    return -IO_EINVAL;

  uint64_t val = nsecs2ticks(*(const uint64_t*)data);
  int module_offset = io->channel * GPTM_MODULE_OFFSET;
  if(val) {
    GPTM_REG(module_offset, GPTM_TAILR) = (uint32_t)val;
    GPTM_REG(module_offset, GPTM_TBILR) = (uint32_t)(val >> 32);

    // enable the timer
    GPTM_REG(module_offset, GPTM_CTL) |= 1;
  }
  else
    GPTM_REG(module_offset, GPTM_CTL) &= ~1;

  return 1;
}

//------------------------------------------------------------------------------
// Read 32-bit timer state
//------------------------------------------------------------------------------
static int32_t timer32_read(IO_io *io, void *data, uint32_t length)
{
  if(length != 1)
    return -IO_EINVAL;

  uint64_t *val = data;
  int module_offset = io->channel * GPTM_MODULE_OFFSET;
  *val = ticks2nsecs(GPTM_REG(module_offset, GPTM_TAILR));

  return 1;
}

//------------------------------------------------------------------------------
// Read 64-bit timer state
//------------------------------------------------------------------------------
static int32_t timer64_read(IO_io *io, void *data, uint32_t length)
{
  if(length != 1)
    return -IO_EINVAL;

  uint64_t *val = data;
  uint64_t tmp = 0;
  int module_offset = io->channel * GPTM_MODULE_OFFSET;
  *val = 0;
  tmp = GPTM_REG(module_offset, GPTM_TAILR);
  *val |= tmp;
  tmp = GPTM_REG(module_offset, GPTM_TBILR);
  *val |= (tmp << 32);
  *val = ticks2nsecs(*val);

  return 1;
}

//------------------------------------------------------------------------------
// Sync timer
//------------------------------------------------------------------------------
static int32_t timer_sync(IO_io *io)
{
  return 0;
}

//------------------------------------------------------------------------------
// Initialize a timer
//------------------------------------------------------------------------------
int32_t IO_timer_init(IO_io *io, uint8_t module)
{
  if(module > 10)
    return -IO_EINVAL;
  return TM4C_timer_init(io, module);
}

//------------------------------------------------------------------------------
// Initialize a timer
//------------------------------------------------------------------------------
int32_t TM4C_timer_init(IO_io *io, uint8_t module)
{
  if(module > 11)
    return -IO_EINVAL;

  uint16_t module_offset = module * GPTM_MODULE_OFFSET;

  //----------------------------------------------------------------------------
  // Set up the hardware
  //----------------------------------------------------------------------------
  // enable the clock
  if(module <= 5)
    RCGCTIMER_REG |= (1 << module);
  else
    RCGCWTIMER_REG |= (1 << (module-6));

  // disable the timer
  GPTM_REG(module_offset, GPTM_CTL) &= ~1;

  // use the maximum size timer
  GPTM_REG(module_offset, GPTM_CFG) = 0;

  // one shot timer
  GPTM_REG(module_offset, GPTM_TAMR) = 0x01;

  // time-out interrupt
  GPTM_REG(module_offset, GPTM_IMR) = 0x01;
  TM4C_enable_interrupt(timer_interrupt[module], 7);

  //----------------------------------------------------------------------------
  // Set up the software
  //----------------------------------------------------------------------------
  io->channel = module;
  io->type    = IO_TIMER;
  io->flags   = IO_ASYNC;
  io->event   = 0;
  io->sync    = timer_sync;

  if(module <= 5) {
    io->read    = timer32_read;
    io->write   = timer32_write;
  }
  else {
    io->read    = timer64_read;
    io->write   = timer64_write;
  }
  timer_devices[module] = io;

  return 0;
}
