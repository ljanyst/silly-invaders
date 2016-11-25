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

#ifndef __IO_IMPL__
#error "This is a low-level header for driver implementation purposes only"
#endif

#include "IO.h"

//------------------------------------------------------------------------------
//! Initialize a display device
//!
//! @param io     the io structure to be initialized
//! @param module number of a display device to be configured
//------------------------------------------------------------------------------
int32_t IO_display_init_low(IO_io *io, uint8_t module);

//------------------------------------------------------------------------------
//! Get parameters of the device
//------------------------------------------------------------------------------
int32_t IO_display_get_attrs_low(IO_io *io, IO_display_attrs *attrs);

//------------------------------------------------------------------------------
//! Get number of display devices available
//------------------------------------------------------------------------------
int32_t IO_display_count_low();

//------------------------------------------------------------------------------
//! Clear the display
//------------------------------------------------------------------------------
int32_t IO_display_clear_low(IO_io *io);
