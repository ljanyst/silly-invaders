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
#include "IO_malloc.h"

#include <string.h>

//------------------------------------------------------------------------------
// Internal display information
//------------------------------------------------------------------------------
struct display {
  const IO_font *font;
  uint16_t width;
  uint16_t height;
  uint16_t line_height;
  uint16_t space_width;
  uint16_t x;
  uint16_t y;
};

typedef struct display display;

static display *displays;

//------------------------------------------------------------------------------
// Read from display
//------------------------------------------------------------------------------
static int32_t display_read(IO_io *io, void *data, uint32_t length)
{
  return -IO_EOPNOTSUPP; // not supported
}

//------------------------------------------------------------------------------
// Write to display
//------------------------------------------------------------------------------
static int32_t display_write(IO_io *io, const void *data, uint32_t length)
{
  display *dsp = &displays[io->channel];
  const char *text = data;

  int i;
  for(i = 0; i < length; ++i) {
    if(text[i] == '\n') {
      dsp->y += dsp->line_height;
      continue;
    }
    else if(text[i] == '\r') {
      dsp->x = 0;
      continue;
    }

    const IO_bitmap *bmp = IO_font_get_glyph(dsp->font, text[i]);
    if(dsp->x + bmp->width > dsp->width) {
      dsp->x = 0;
      dsp->y += dsp->line_height;
    }
    if(dsp->y + bmp->height > dsp->height)
      break;

    IO_display_print_bitmap(io, dsp->x, dsp->y, bmp);
    dsp->x += bmp->width;
  }
  return i;
}

//------------------------------------------------------------------------------
// Initialize a display device
//------------------------------------------------------------------------------
int32_t IO_display_init(IO_io *io, uint8_t module)
{
  //----------------------------------------------------------------------------
  // First run
  //----------------------------------------------------------------------------
  if(!displays) {
    int32_t num = IO_display_count_low();
    if(num < 1)
      return -IO_EINVAL;
    displays = IO_malloc(num*sizeof(display));
    memset(displays, 0, num*sizeof(display));
  }

  //----------------------------------------------------------------------------
  // Initialize the display
  //----------------------------------------------------------------------------
  uint32_t ret = IO_display_init_low(io, module);
  if(ret)
    return ret;

  IO_display_attrs attrs;
  IO_display_get_attrs(io, &attrs);

  display *dsp = &displays[module];
  dsp->width = attrs.width;
  dsp->height = attrs.height;

  //----------------------------------------------------------------------------
  // Set the font
  //----------------------------------------------------------------------------
  const IO_font *font = IO_font_get_by_name("DejaVuSans10");
  if(!font)
    return -IO_EIO;

  IO_display_set_font(io, font);

  //----------------------------------------------------------------------------
  // Add read and write functions
  //----------------------------------------------------------------------------
  io->read  = display_read;
  io->write = display_write;

  return 0;
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
  if(io->type != IO_DISPLAY)
    return -IO_EINVAL;
  attrs->line_height = displays[io->channel].line_height;
  attrs->space_width = displays[io->channel].space_width;
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
int32_t IO_display_clear(IO_io *io)
{
  displays[io->channel].x = 0;
  displays[io->channel].y = 0;
  return IO_display_clear_low(io);
}

//------------------------------------------------------------------------------
// Clear the display
//------------------------------------------------------------------------------
int32_t __IO_display_clear_low(IO_io *io)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_display_clear_low, IO_display_clear_low);

//------------------------------------------------------------------------------
// Put a pixel on the screen
//------------------------------------------------------------------------------
int32_t __IO_display_put_pixel(IO_io *io, uint16_t x, uint16_t y, uint32_t argb)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_display_put_pixel, IO_display_put_pixel);

//------------------------------------------------------------------------------
// Print bitmap
//------------------------------------------------------------------------------
int32_t __IO_display_print_bitmap(IO_io *io, uint16_t x, uint16_t y,
  const IO_bitmap *bitmap)
{
  const char *data = bitmap->data;
  uint16_t w = bitmap->width;
  for(int i = 0; i < w; ++i)
    for(int j = 0; j < bitmap->height; ++j)
      IO_display_put_pixel(io, x+i, y+j, ARR2D(data, i, j, w));
  return 0;
}

WEAK_ALIAS(__IO_display_print_bitmap, IO_display_print_bitmap);

//------------------------------------------------------------------------------
// Get number of display devices available
//------------------------------------------------------------------------------
int32_t __IO_display_count_low()
{
  return 0;
}

WEAK_ALIAS(__IO_display_count_low, IO_display_count_low);

//------------------------------------------------------------------------------
// Set font for the display
//------------------------------------------------------------------------------
int32_t IO_display_set_font(IO_io *io, const IO_font *font)
{
  if(io->type != IO_DISPLAY)
    return -IO_EINVAL;

  displays[io->channel].font = font;
  displays[io->channel].line_height = font->size+1;
  displays[io->channel].space_width = font->glyphs[0]->width;
  return 0;
}

//------------------------------------------------------------------------------
// Move the cursor to given coordinate
//------------------------------------------------------------------------------
int32_t IO_display_cursor_goto(IO_io *io, uint32_t x, uint32_t y)
{
  if(io->type != IO_DISPLAY)
    return -IO_EINVAL;

  display *dsp = &displays[io->channel];
  dsp->x  = x;
  dsp->y  = y;
  dsp->x %= dsp->width;
  dsp->y %= dsp->height;
  return 0;
}

//------------------------------------------------------------------------------
// Move the cursor to given line
//------------------------------------------------------------------------------
int32_t IO_display_cursor_goto_text(IO_io *io, uint32_t line, uint32_t space)
{
  if(io->type != IO_DISPLAY)
    return -IO_EINVAL;

  display *dsp = &displays[io->channel];
  dsp->y  = line * dsp->line_height;
  dsp->x  = space * dsp->space_width;
  dsp->x %= dsp->width;
  dsp->y %= dsp->height;
  return 0;
}

//------------------------------------------------------------------------------
// Move the cursor relative to the current location
//------------------------------------------------------------------------------
int32_t IO_display_cursor_move(IO_io *io, int32_t dx, int32_t dy)
{
  if(io->type != IO_DISPLAY)
    return -IO_EINVAL;

  display *dsp = &displays[io->channel];
  dsp->x += dx;
  dsp->y += dy;
  dsp->x %= dsp->width;
  dsp->y %= dsp->height;
  return 0;
}

//------------------------------------------------------------------------------
// Move the cursor relative to the current location by lines and spaces
//------------------------------------------------------------------------------
int32_t IO_display_cursor_move_text(IO_io *io, int32_t dline, int32_t dspace)
{
  if(io->type != IO_DISPLAY)
    return -IO_EINVAL;

  display *dsp = &displays[io->channel];
  dsp->y += dline * dsp->line_height;
  dsp->x += dspace * dsp->space_width;
  dsp->x %= dsp->width;
  dsp->y %= dsp->height;
  return 0;
}
