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

#include "IO.h"
#include "IO_error.h"
#include "IO_utils.h"

//------------------------------------------------------------------------------
// Dummu IO initializer
//------------------------------------------------------------------------------
int32_t __IO_init()
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_init, IO_init);

//------------------------------------------------------------------------------
// Write data to an output device
//------------------------------------------------------------------------------
int32_t IO_write(IO_output *out, const void *data, uint32_t length)
{
  int32_t i = 0;
  const uint8_t *b_data = data;
  for(i = 0; i < length; ++i) {
    int ret = (*out->put_byte)(out, b_data[i]);
    if(ret) return ret;
  }
  return i;
}

//------------------------------------------------------------------------------
// Read data from an input device
//------------------------------------------------------------------------------
int32_t IO_read(IO_input *in, void *data, uint32_t length)
{
  int32_t i = 0;
  uint8_t *b_data = data;
  for(i = 0; i < length; ++i) {
    int ret = (*in->get_byte)(in, &b_data[i]);
    if(ret) return ret;
  }
  return i;
}
