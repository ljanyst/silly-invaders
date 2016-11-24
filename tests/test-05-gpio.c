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
#include <io/IO_sys.h>
#include <io/IO_device.h>

#define PULLUP_REG (*(volatile unsigned long *)0x4005d510)
IO_io led[2];
IO_io button[2];

void button_event(IO_io *io, uint16_t event)
{
  int btn = 0;
  uint64_t state;
  if(io == &button[1]) btn = 1;

  IO_get(&button[btn], &state);
  if(!state)
    IO_set(&led[btn], 1);
  else
    IO_set(&led[btn], 0);
}

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init(4096);
  IO_gpio_init(&led[0], 41, 0, 1);
  IO_gpio_init(&led[1], 42, 0, 1);
  IO_gpio_init(&button[0], 40, IO_ASYNC, 0);
  IO_gpio_init(&button[1], 44, IO_ASYNC, 0);
  button[0].event = button_event;
  button[1].event = button_event;
  PULLUP_REG |= 0x11;
  IO_event_enable(&button[0], IO_EVENT_CHANGE);
  IO_event_enable(&button[1], IO_EVENT_CHANGE);

  while(1)
    IO_wait_for_interrupt();
}
