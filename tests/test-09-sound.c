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
#include <io/IO_sound.h>
#include <string.h>

IO_io display;
IO_io button[2];
IO_io sound;
IO_io timer;
uint64_t val = 0;
uint8_t stat[2] = {0, 0};

const char *entertainer = "Entertainer:d=4,o=5,b=140:8d,8d#,8e,c6,8e,c6,8e,"
  "2c.6,8c6,8d6,8d#6,8e6,8c6,8d6,e6,8b,d6,2c6,p,8d,8d#,8e,c6,8e,c6,8e,2c.6,"
  "8p,8a,8g,8f#,8a,8c6,e6,8d6,8c6,8a,2d6";

const char *pink_panther = "PinkPanther:d=4,o=5,b=160:8d#,8e,2p,8f#,8g,2p,"
  "8d#,8e,16p,8f#,8g,16p,8c6,8b,16p,8d#,8e,16p,8b,2a#,2p,16a,16g,16e,16d,2e";

const char *indiana = "Indiana:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,"
  "1f,p.,g,8p,8a,8b,8p,1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,"
  "8e6,1f.6,g,8p,8g,e.6,8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6";

const char *adams = "aadams:d=4,o=5,b=160:8c,f,8a,f,8c,b4,2g,8f,e,8g,e,8e4,a4,"
  "2f,8c,f,8a,f,8c,b4,2g,8f,e,8c,d,8e,1f,8c,8d,8e,8f,1p,8d,8e,8f#,8g,1p,8d,8e,"
  "8f#,8g,p,8d,8e,8f#,8g,p,8c,8d,8e,8f";

IO_tune *tunes[4];
const char *names[] = {
  "Entertainer", "Pink Panther", "Indiana Jones", "The Adams Family"};

//------------------------------------------------------------------------------
// Display results
//------------------------------------------------------------------------------
void show()
{
  IO_display_clear(&display);
  IO_print(&display, "Tune:\r\n%s", names[val]);
  IO_sync(&display);
}

//------------------------------------------------------------------------------
// An button event
//------------------------------------------------------------------------------
void button_event(IO_io *io, uint16_t event)
{
  if(io == &button[0]) {
    if(!stat[0]) {
      --val;
      stat[0] = 1;
    }
    else
      stat[0] = 0;
  }
  else {
    if(!stat[1]) {
      ++val;
      stat[1] = 1;
    }
    else
      stat[1] = 0;
  }
  val %= 4;
  IO_sound_play(&sound, &timer, tunes[val], 0);
  show();
}

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init();
  IO_display_init(&display, 0);
  IO_sound_init(&sound, 0);
  IO_timer_init(&timer, 0);

  IO_button_init(&button[0], 0, IO_ASYNC);
  IO_button_init(&button[1], 1, IO_ASYNC);

  button[0].event = button_event;
  button[1].event = button_event;

  IO_event_enable(&button[0], IO_EVENT_CHANGE);
  IO_event_enable(&button[1], IO_EVENT_CHANGE);

  show();

  tunes[0] = IO_sound_decode_RTTTL(entertainer);
  tunes[1] = IO_sound_decode_RTTTL(pink_panther);
  tunes[2] = IO_sound_decode_RTTTL(indiana);
  tunes[3] = IO_sound_decode_RTTTL(adams);
  IO_sound_play(&sound, &timer, tunes[0], 0);

  while(1)
    IO_wait_for_interrupt();
}
