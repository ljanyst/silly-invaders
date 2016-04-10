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

//------------------------------------------------------------------------------
// SSI frame formats
//------------------------------------------------------------------------------
#define SSI_FRAME_TEXAS      1
#define SSI_FRAME_FREESCALE  2
#define SSI_FRAME_MICROWIRE  3

//------------------------------------------------------------------------------
// SSI attributes
//------------------------------------------------------------------------------
struct IO_ssi_attrs {
  uint8_t  master;        //!< 1 for master, 0 for slave
  uint8_t  slave_out;     //!< 1 slave output enabled, 0 slave output disabled
  uint32_t bandwidth;     //!< bandwidth in bps
  uint8_t  frame_format;  //!< frame format
  uint8_t  freescale_spo; //!< SPO value for freescale frames
  uint8_t  freescale_sph; //!< SPH value for freescale frames
  uint8_t  frame_size;    //!< size of the frame in bits
};

typedef struct IO_ssi_attrs IO_ssi_attrs;

//------------------------------------------------------------------------------
// Open SSI device and create the associated input and output structures
//
// @param io     the io structure to be initialized
// @param module UART module number to be configured
// @param flags  flags
// @param clock  clock speed
//------------------------------------------------------------------------------
int32_t IO_ssi_init(IO_io *io, uint8_t module, uint16_t flags,
  IO_ssi_attrs *attrs);
