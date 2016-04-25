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

#include <io/IO.h>
#include <io/IO_device.h>
#include <io/IO_error.h>
#include <io/IO_display.h>
#include <io/IO_display_low.h>

#include "TM4C.h"

#include <drivers/pcd8544/pcd8544.h>

//------------------------------------------------------------------------------
// Initialize the platform
//------------------------------------------------------------------------------
int32_t IO_init()
{
  return TM4C_init();
}

//------------------------------------------------------------------------------
// Initialize a button
//------------------------------------------------------------------------------
int32_t IO_button_init(IO_io *io, uint8_t module, uint16_t flags)
{
  if(module > 1)
    return -IO_EINVAL;

  if(module == 0)
    return IO_gpio_init(io, 40, flags, 0);

  return IO_gpio_init(io, 44, flags, 0);
}

//------------------------------------------------------------------------------
// Initialize an LED
//------------------------------------------------------------------------------
int32_t IO_led_init(IO_io *io, uint8_t module)
{
  if(module > 0)
    return -IO_EINVAL;

  return IO_gpio_init(io, 37, 0, 1);
}

//------------------------------------------------------------------------------
// Initialize a slider
//------------------------------------------------------------------------------
int32_t IO_slider_init(IO_io *io, uint8_t module, uint16_t flags)
{
  if(module > 0)
    return -IO_EINVAL;

  return IO_adc_init(io, 0, flags);
}

//------------------------------------------------------------------------------
// The display structs
//------------------------------------------------------------------------------
static pcd8544 display0;

//------------------------------------------------------------------------------
// Write the pixel matrix to the device
//------------------------------------------------------------------------------
static int32_t display_sync(IO_io *io)
{
  return PCD8544_sync(&display0);
}

//------------------------------------------------------------------------------
// Initialize a display device
//------------------------------------------------------------------------------
int32_t IO_display_init_low(IO_io *io, uint8_t module)
{
  if(module != 0)
    return -IO_EINVAL;

  //----------------------------------------------------------------------------
  // Initialize the display 0
  //----------------------------------------------------------------------------
  io->type    = IO_DISPLAY;
  io->sync    = display_sync;
  io->channel = 0;
  io->flags   = 0;
  io->read    = 0;
  io->write   = 0;
  io->event   = 0;
  return PCD8544_init(&display0, 0, 6, 7);
}

//------------------------------------------------------------------------------
// Get parameters of the device
//------------------------------------------------------------------------------
int32_t IO_display_get_attrs_low(IO_io *io, IO_display_attrs *attrs)
{
  if(io->type != IO_DISPLAY || io->channel != 0)
    return -IO_EINVAL;

  return PCD8544_get_attrs(attrs);
}

//------------------------------------------------------------------------------
// Clear the display
//------------------------------------------------------------------------------
int32_t IO_display_clear_low(IO_io *io)
{
  if(io->type != IO_DISPLAY || io->channel != 0)
    return -IO_EINVAL;

  return PCD8544_clear(&display0);
}

//------------------------------------------------------------------------------
// Put a pixel on the screen
//------------------------------------------------------------------------------
int32_t IO_display_put_pixel(IO_io *io, uint16_t x, uint16_t y, uint32_t argb)
{
  if(io->type != IO_DISPLAY || io->channel != 0)
    return -IO_EINVAL;

  return PCD8544_put_pixel(&display0, x, y, argb);
}

//------------------------------------------------------------------------------
// Get number of display devices available
//------------------------------------------------------------------------------
int32_t IO_display_count_low()
{
  return 1;
}
