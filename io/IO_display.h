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

#pragma once

#include "IO.h"
#include "IO_font.h"

//------------------------------------------------------------------------------
//! Initialize a display device
//!
//! @param io     the io structure to be initialized
//! @param module number of a display device to be configured
//------------------------------------------------------------------------------
int32_t IO_display_init(IO_io *io, uint8_t module);

//------------------------------------------------------------------------------
//! Display attributes
//------------------------------------------------------------------------------
struct IO_display_attrs {
  uint16_t width;       //!< width in pixels
  uint16_t height;      //!< height in pixels
  uint16_t line_height; //!< line height
  uint16_t space_width; //!< space width
  uint8_t  color_depth; //!< color depth in bits per pixel
};

typedef struct IO_display_attrs IO_display_attrs;

//------------------------------------------------------------------------------
//! Get parameters of the device
//------------------------------------------------------------------------------
int32_t IO_display_get_attrs(IO_io *io, IO_display_attrs *attrs);

//------------------------------------------------------------------------------
//! Clear the display
//------------------------------------------------------------------------------
int32_t IO_display_clear(IO_io *io);

//------------------------------------------------------------------------------
//! Put a pixel on the screen
//!
//! @param io   the IO device
//! @param x    x coordinate
//! @param y    y coordinate
//! @param argb color of the pixel in ARGB mode
//------------------------------------------------------------------------------
int32_t IO_display_put_pixel(IO_io *io, uint16_t x, uint16_t y, uint32_t argb);

//------------------------------------------------------------------------------
//! Bitmap
//------------------------------------------------------------------------------
struct IO_bitmap {
  uint16_t  width;   //!< width
  uint16_t  height;  //!< height
  uint8_t   bpp;     //!< bits per pixel
  void     *data;    //!< flattened data
};

typedef struct IO_bitmap IO_bitmap;

//------------------------------------------------------------------------------
//! Print bitmap
//------------------------------------------------------------------------------
int32_t IO_display_print_bitmap(IO_io *io, uint16_t x, uint16_t y,
  const IO_bitmap *bitmap);

//------------------------------------------------------------------------------
//! Set font for the display
//------------------------------------------------------------------------------
int32_t IO_display_set_font(IO_io *io, const IO_font *font);
