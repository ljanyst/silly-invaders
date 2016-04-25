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
//! Open an UART device and create the associated IO structure
//!
//! @param io     the io structure to be initialized
//! @param module UART module number to be configured
//! @param flags  flags
//! @param baud   baud rate of the device
//------------------------------------------------------------------------------
int32_t IO_uart_init(IO_io *io, uint8_t module, uint16_t flags, uint32_t baud);

//------------------------------------------------------------------------------
//! Initialize a timer
//!
//! @param io     the io structure to be initialized
//! @param module number of a timer device to be configured
//------------------------------------------------------------------------------
int32_t IO_timer_init(IO_io *io, uint8_t module);

//------------------------------------------------------------------------------
// SSI frame formats
//------------------------------------------------------------------------------
#define SSI_FRAME_TEXAS      1
#define SSI_FRAME_FREESCALE  2
#define SSI_FRAME_MICROWIRE  3

//------------------------------------------------------------------------------
//! SSI attributes
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
//! Open SSI device and create the associated input and output structures
//!
//! @param io     the io structure to be initialized
//! @param module SSI module number to be configured
//! @param flags  flags
//! @param attrs  SSI attributes
//------------------------------------------------------------------------------
int32_t IO_ssi_init(IO_io *io, uint8_t module, uint16_t flags,
  IO_ssi_attrs *attrs);

//------------------------------------------------------------------------------
//! Initialize an LED, returns the appropriate GPIO
//!
//! @param io     the io structure to be initialized
//! @param module number of the LED device to be configured
//------------------------------------------------------------------------------
int32_t IO_led_init(IO_io *io, uint8_t module);

//------------------------------------------------------------------------------
//! Initialize a GPIO pin
//!
//! @param io    the io structure to be initialized
//! @param pin   number of the pin to initialize
//! @param flags flags, IO_ASYNC can be passed to receive edge events
//! @param dir   0 - input, 1 - output
//------------------------------------------------------------------------------
int32_t IO_gpio_init(IO_io *io, uint8_t pin, uint16_t flags, uint8_t dir);

//------------------------------------------------------------------------------
//! Initialize a button, returns the appropriate GPIO
//!
//! @param io     the io structure to be initialized
//! @param module number of a display device to be configured
//! @param flags  flags
//------------------------------------------------------------------------------
int32_t IO_button_init(IO_io *io, uint8_t module, uint16_t flags);

//------------------------------------------------------------------------------
//! Initialize an ADC
//!
//! @param io     the io structure to be initialized
//! @param module number of the ADC device to be configured
//! @param flags  flags
//------------------------------------------------------------------------------
int32_t IO_adc_init(IO_io *io, uint8_t module, uint16_t flags);

//------------------------------------------------------------------------------
//! Initialize a slider
//!
//! @param io     the io structure to be initialized
//! @param module number of the ADC device to be configured
//! @param flags  flags
//------------------------------------------------------------------------------
int32_t IO_slider_init(IO_io *io, uint8_t module, uint16_t flags);
