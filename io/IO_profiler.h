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
//! Add a channel to the profiler
//!
//! @param num    number of the channel (0-15)
//! @param gpio   the io device to toggle
//------------------------------------------------------------------------------
int32_t IO_profiler_channel_add(uint8_t num, IO_io *gpio);

//------------------------------------------------------------------------------
//! Toggle the profiler channel
//!
//! @param num number of the channel to toggle
//------------------------------------------------------------------------------
int32_t IO_profiler_toggle(uint8_t num);
