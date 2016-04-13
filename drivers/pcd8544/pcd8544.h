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

#include <io/IO.h>
#include <io/IO_display.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// PCD8544 device
//------------------------------------------------------------------------------
struct pcd8544 {
  IO_io reset;            //! Reset GPIO
  IO_io dc;               //! Data/~Command GPIO
  IO_io ssi;              //! Communication interface
  uint8_t pixels[6][84];  //! Pixel matrix
};

typedef struct pcd8544 pcd8544;

//------------------------------------------------------------------------------
// Initialize the device
//------------------------------------------------------------------------------
int32_t PCD8544_init(pcd8544 *device, uint8_t ssi_module, uint8_t dc_pin,
  uint8_t reset_pin);

//------------------------------------------------------------------------------
// Get attributes
//------------------------------------------------------------------------------
int32_t PCD8544_get_attrs(IO_display_attrs *attrs);

//------------------------------------------------------------------------------
// Clear all the pixels
//------------------------------------------------------------------------------
int32_t PCD8544_clear(pcd8544 *device);

//------------------------------------------------------------------------------
// Put a pixel
//------------------------------------------------------------------------------
int32_t PCD8544_put_pixel(pcd8544 *device, uint16_t x, uint16_t y,
  uint32_t argb);

//------------------------------------------------------------------------------
// Write the picel matrix to the device
//------------------------------------------------------------------------------
int32_t PCD8544_sync(pcd8544 *device);
