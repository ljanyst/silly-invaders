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
#include <io/IO_device.h>

IO_io led[2];
IO_io timer;
int state = 0;

void timer_event(IO_io *io, uint16_t event)
{
  if(state) {
    state = 0;
    IO_set(&led[0], 0);
    IO_set(&led[1], 1);
  }
  else {
    state = 1;
    IO_set(&led[1], 0);
    IO_set(&led[0], 1);
  }
  IO_set(&timer, 500000000); // fire in half second
}

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init(4096);
  IO_gpio_init(&led[0], 41, 0, 1);
  IO_gpio_init(&led[1], 42, 0, 1);
  IO_timer_init(&timer, 0);
  timer.event = timer_event;
  timer_event(0, 0);

  while(1)
    IO_wait_for_interrupt();
}
