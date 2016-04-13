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

#include "IO_display.h"
#include "IO_display_low.h"
#include "IO_error.h"
#include "IO_utils.h"

//------------------------------------------------------------------------------
// Initialize a display device
//------------------------------------------------------------------------------
int32_t IO_display_init(IO_io *io, uint8_t module)
{
  return IO_display_init_low(io, module);
}

//------------------------------------------------------------------------------
// Initialize a display device
//------------------------------------------------------------------------------
int32_t __IO_display_init_low(IO_io *io, uint8_t module)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_display_init_low, IO_display_init_low);

//------------------------------------------------------------------------------
// Get parameters of the device
//------------------------------------------------------------------------------
int32_t IO_display_get_attrs(IO_io *io, IO_display_attrs *attrs)
{
  return IO_display_get_attrs_low(io, attrs);
}

//------------------------------------------------------------------------------
// Get parameters of the device
//------------------------------------------------------------------------------
int32_t __IO_display_get_attrs_low(IO_io *io, IO_display_attrs *attrs)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_display_get_attrs_low, IO_display_get_attrs_low);

//------------------------------------------------------------------------------
// Clear the display
//------------------------------------------------------------------------------
int32_t __IO_display_clear(IO_io *io)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_display_clear, IO_display_clear);

//------------------------------------------------------------------------------
// Put a pixel on the screen
//------------------------------------------------------------------------------
int32_t __IO_display_put_pixel(IO_io *io, uint16_t x, uint16_t y, uint32_t argb)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_display_put_pixel, IO_display_put_pixel);
