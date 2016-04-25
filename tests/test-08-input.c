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

IO_io display;
IO_io button[2];
IO_io slider;
IO_io timer;
uint64_t sliderR = 0;

//------------------------------------------------------------------------------
// Display results
//------------------------------------------------------------------------------
void show()
{
  uint64_t btn0;
  uint64_t btn1;

  IO_get(&button[0], &btn0);
  IO_get(&button[1], &btn1);
  IO_display_clear(&display);
  IO_print(&display, "Btn #0: %lld\r\n", btn0);
  IO_print(&display, "Btn #1: %lld\r\n", btn1);
  IO_print(&display, "ADC #0: %lld\r\n", sliderR);

  // We should not do it in an interrupt handler, but since there is
  // nothing elso going on, then who cares.
  IO_sync(&display);
}

//------------------------------------------------------------------------------
// A timer event
//------------------------------------------------------------------------------
void timer_event(IO_io *io, uint16_t event)
{
  IO_set(&slider, 0); // request a sample
}

//------------------------------------------------------------------------------
// An ADC event
//------------------------------------------------------------------------------
void slider_event(IO_io *io, uint16_t event)
{
  IO_get(&slider, &sliderR);
  IO_set(&timer, 100000000); // fire in 0.1 sec
  show();
}

//------------------------------------------------------------------------------
// An button event
//------------------------------------------------------------------------------
void button_event(IO_io *io, uint16_t event)
{
  show();
}

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init();
  IO_display_init(&display, 0);

  IO_button_init(&button[0], 0, IO_ASYNC);
  IO_button_init(&button[1], 1, IO_ASYNC);
  IO_timer_init(&timer, 0);
  IO_slider_init(&slider, 0, IO_ASYNC);

  button[0].event = button_event;
  button[1].event = button_event;
  timer.event     = timer_event;
  slider.event    = slider_event;

  IO_event_enable(&button[0], IO_EVENT_CHANGE);
  IO_event_enable(&button[1], IO_EVENT_CHANGE);
  IO_event_enable(&slider,    IO_EVENT_DONE);

  IO_set(&slider, 0); // request a sample

  show();

  while(1)
    IO_wait_for_interrupt();
}
