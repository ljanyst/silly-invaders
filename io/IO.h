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

#include <stdint.h>

//------------------------------------------------------------------------------
// Initialize the IO modules
//------------------------------------------------------------------------------
int32_t IO_init();

//------------------------------------------------------------------------------
// IO Output
//------------------------------------------------------------------------------
struct IO_output {
  int32_t (*put_byte)(struct IO_output *out, uint8_t byte);
  void *data;
};
typedef struct IO_output IO_output;

int32_t IO_write(IO_output *out, const void *data, uint32_t length);

//------------------------------------------------------------------------------
// IO Input
//------------------------------------------------------------------------------
struct IO_input {
  int32_t (*get_byte)(struct IO_input *in, uint8_t *byte);
  void *data;
};
typedef struct IO_input IO_input;

int32_t IO_read(IO_input *in, void *data, uint32_t length);
