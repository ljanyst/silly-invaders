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
#include <io/IO_display.h>
#include <string.h>

IO_io display;
IO_io button[2];
uint8_t stat[2] = {0, 0};
int seeded = 0;

//------------------------------------------------------------------------------
// Display results
//------------------------------------------------------------------------------
void show()
{
  IO_display_clear(&display);
  IO_print(&display, "RNG:\r\n%u\r\nTime:\r\n%llu", IO_random(), IO_time());
  IO_sync(&display);
}

//------------------------------------------------------------------------------
// An button event
//------------------------------------------------------------------------------
void button_event(IO_io *io, uint16_t event)
{
  if(!seeded) {
    IO_rng_seed(IO_time());
    seeded = 1;
  }
  show();
}

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init(4096);
  IO_display_init(&display, 0);

  IO_button_init(&button[0], 0, IO_ASYNC);
  IO_button_init(&button[1], 1, IO_ASYNC);

  button[0].event = button_event;
  button[1].event = button_event;

  IO_event_enable(&button[0], IO_EVENT_CHANGE);
  IO_event_enable(&button[1], IO_EVENT_CHANGE);

  show();

  while(1)
    IO_wait_for_interrupt();
}
