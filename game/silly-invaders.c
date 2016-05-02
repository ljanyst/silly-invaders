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
#include <io/IO_font.h>
#include <string.h>

//------------------------------------------------------------------------------
// Bitmaps
//------------------------------------------------------------------------------
extern const IO_bitmap BunkerDamagedImg;
extern const IO_bitmap BunkerImg;
extern const IO_bitmap DefenderImg;
extern const IO_bitmap HeartImg;
extern const IO_bitmap Invader1Img;
extern const IO_bitmap Invader2Img;
extern const IO_bitmap Invader3Img;
extern const IO_bitmap Invader4Img;

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init();
  IO_io display;
  IO_display_init(&display, 0);

  const IO_font *font = IO_font_get_by_name("SilkScreen8");
  IO_display_set_font(&display, font);

  IO_print(&display, "0123456789");

  IO_display_print_bitmap(&display, 0, 10, &BunkerDamagedImg);
  IO_display_print_bitmap(&display, 12, 10, &BunkerImg);
  IO_display_print_bitmap(&display, 24, 10, &DefenderImg);
  IO_display_print_bitmap(&display, 36, 10, &HeartImg);
  IO_display_print_bitmap(&display, 0, 30, &Invader1Img);
  IO_display_print_bitmap(&display, 12, 30, &Invader2Img);
  IO_display_print_bitmap(&display, 24, 30, &Invader3Img);
  IO_display_print_bitmap(&display, 36, 30, &Invader4Img);
  IO_sync(&display);

  while(1)
    IO_wait_for_interrupt();
}
