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

#include <io/IO_error.h>
#include <io/IO_device.h>
#include "pcd8544.h"

//------------------------------------------------------------------------------
// Initialization sequence
//------------------------------------------------------------------------------
static const uint8_t init_seq[] = {
  0x21,  // extended commands
  0xb1,  // voltage
  0x04,  // temperature coefficient
  0x14,  // bias
  0x20,  // basic commands
  0x0c   // normal video mode
};

//------------------------------------------------------------------------------
// Cursor reset sequence
//------------------------------------------------------------------------------
static const uint8_t cursor_rst_seq[] = {
  0x20,  // basic commands
  0x40,  // y cursor to 0
  0x80,  // x cursor to 0
};

//------------------------------------------------------------------------------
// Initialize the device
//------------------------------------------------------------------------------
int32_t PCD8544_init(pcd8544 *device, uint8_t ssi_module, uint8_t dc_pin,
  uint8_t reset_pin)
{
  //----------------------------------------------------------------------------
  // Initialize the actual IO
  //----------------------------------------------------------------------------
  IO_ssi_attrs attrs;
  attrs.bandwidth     = 3900000;
  attrs.frame_size    = 8;
  attrs.frame_format  = SSI_FRAME_FREESCALE;
  attrs.freescale_spo = 1;
  attrs.freescale_sph = 1;
  attrs.master        = 1;

  IO_ssi_init(&device->ssi, ssi_module, 0, &attrs);

  IO_gpio_init(&device->dc, dc_pin, 0, 1);
  IO_gpio_init(&device->reset, reset_pin, 0, 1);

  //----------------------------------------------------------------------------
  // Reset the device
  //----------------------------------------------------------------------------
  IO_set(&device->reset, 0);
  for(volatile int i = 0; i < 10; ++i);
  IO_set(&device->reset, 1);

  //----------------------------------------------------------------------------
  // Initialize and set cursor to (0, 0)
  //----------------------------------------------------------------------------
  IO_set(&device->dc, 0);
  IO_write(&device->ssi, init_seq, sizeof(init_seq));
  IO_write(&device->ssi, cursor_rst_seq, sizeof(cursor_rst_seq));
  IO_sync(&device->ssi);
  IO_set(&device->dc, 1);

  return 0;
}

//------------------------------------------------------------------------------
// Get attributes
//------------------------------------------------------------------------------
int32_t PCD8544_get_attrs(IO_display_attrs *attrs)
{
  attrs->width       = 84;
  attrs->height      = 48;
  attrs->color_depth = 1;
  return 0;
}

//------------------------------------------------------------------------------
// Clear all the pixels
//------------------------------------------------------------------------------
int32_t PCD8544_clear(pcd8544 *device)
{
  for(int i = 0; i < 6; ++i)
    for(int j = 0; j < 84; ++j)
      device->pixels[i][j] = 0;
  return 0;
}

//------------------------------------------------------------------------------
// Put a pixel
//------------------------------------------------------------------------------
int32_t PCD8544_put_pixel(pcd8544 *device, uint16_t x, uint16_t y,
  uint32_t argb)
{
  // we should really do this and then cut in half, but let's stay on the
  // simple side
  // https://stackoverflow.com/questions/687261/converting-rgb-to-grayscale-intensity
  if(x > 83 || y > 47)
    return -IO_EINVAL;

  uint8_t ybyte = y / 8;
  uint8_t ybit  = y % 8;
  uint8_t byte  = (1 << ybit);

  // black
  if(!argb)
    device->pixels[ybyte][x] |= byte;
  // other
  else
    device->pixels[ybyte][x] &= ~byte;

  return 0;
}

//------------------------------------------------------------------------------
// Write the picel matrix to the device
//------------------------------------------------------------------------------
int32_t PCD8544_sync(pcd8544 *device)
{
  IO_set(&device->dc, 0);
  IO_write(&device->ssi, cursor_rst_seq, sizeof(cursor_rst_seq));
  IO_sync(&device->ssi);
  IO_set(&device->dc, 1);
  for(int i = 0; i < 6; ++i)
    IO_write(&device->ssi, device->pixels[i], 84);
  IO_sync(&device->ssi);
  IO_set(&device->dc, 0);
  return 0;
}
