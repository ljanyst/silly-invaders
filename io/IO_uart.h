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
// Open UART device and create the associated input and output structures
//
// @param module UART module number to be configured
// @param flags  flags
// @param baud   baud rate of the device
// @param input  input structure to be filled
// @param output output structure to be filled
//------------------------------------------------------------------------------
int32_t IO_uart_init(uint8_t module, uint16_t flags, uint32_t baud,
  IO_input *input, IO_output *output);
