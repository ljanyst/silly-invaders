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

#include "IO_profiler.h"
#include "IO_error.h"

//------------------------------------------------------------------------------
// Profiler state
//------------------------------------------------------------------------------
IO_io *prf_channels[16];
uint16_t prf_state = 0;

//------------------------------------------------------------------------------
// Add a channel to the profiler
//------------------------------------------------------------------------------
int32_t IO_profiler_channel_add(uint8_t num, IO_io *gpio)
{
  if(gpio->type != IO_GPIO || num > 15)
    return -IO_EINVAL;
  prf_channels[num] = gpio;
  IO_set(prf_channels[num], 0);
  prf_state &= ~(1 << num);
  return 0;
}

//------------------------------------------------------------------------------
// Toggle the profiler channel
//------------------------------------------------------------------------------
int32_t IO_profiler_toggle(uint8_t num)
{
  if(num > 15)
    return -IO_EINVAL;

  if(prf_state & (1 << num)) {
    IO_set(prf_channels[num], 0);
    prf_state &= ~(1 << num);
  }
  else {
    IO_set(prf_channels[num], 1);
    prf_state |= (1 << num);
  }
  return 0;
}
