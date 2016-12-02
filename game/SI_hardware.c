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
#include <io/IO_display.h>
#include <io/IO_device.h>
#include <io/IO_sound.h>

#include "SI_hardware.h"

//------------------------------------------------------------------------------
// Devices
//------------------------------------------------------------------------------
IO_io display;
IO_io scene_timer;
IO_io slider;
IO_io slider_timer;
IO_io button[2];
IO_io sound;
IO_sound_player sound_player;
IO_io led;

//------------------------------------------------------------------------------
// Hardware variables
//------------------------------------------------------------------------------
uint64_t         slider_value = 0;
uint64_t         button_value = 0;
IO_display_attrs display_attrs;
uint8_t          rng_initialized;

//------------------------------------------------------------------------------
// Slider timer event
//------------------------------------------------------------------------------
void slider_timer_event(IO_io *io, uint16_t event)
{
  IO_set(&slider, 0); // request a sample
}

//------------------------------------------------------------------------------
// Slider event
//------------------------------------------------------------------------------
void slider_event(IO_io *io, uint16_t event)
{
  IO_get(&slider, &slider_value);
  IO_set(&slider_timer, 20000000); // fire in 0.04 sec
}

//------------------------------------------------------------------------------
// An button event
//------------------------------------------------------------------------------
void button_event(IO_io *io, uint16_t event)
{
  uint64_t btn;
  IO_get(io, &btn);
  if(btn)
    button_value = 1;

  if(!rng_initialized) {
    rng_initialized = 1;
    IO_rng_seed(IO_time());
  }
}

//------------------------------------------------------------------------------
//! Initialize the hardware
//------------------------------------------------------------------------------
void SI_hardware_init()
{
  IO_init(0);

  IO_display_init(&display, 0);
  IO_display_get_attrs(&display, &display_attrs);

  IO_timer_init(&slider_timer, 1);
  IO_slider_init(&slider, 0, IO_ASYNC);
  slider_timer.event = slider_timer_event;
  slider.event       = slider_event;
  IO_event_enable(&slider, IO_EVENT_DONE);
  IO_set(&slider, 0); // request a sample

  IO_button_init(&button[0], 0, IO_ASYNC);
  IO_button_init(&button[1], 1, IO_ASYNC);
  button[0].event = button_event;
  button[1].event = button_event;
  IO_event_enable(&button[0], IO_EVENT_CHANGE);
  IO_event_enable(&button[1], IO_EVENT_CHANGE);

  IO_sound_init(&sound, 0);
  IO_sound_player_init(&sound_player, &sound);

  IO_led_init(&led, 0);
}
