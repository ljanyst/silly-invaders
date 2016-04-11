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

#include "IO_gpio.h"
#include "IO_error.h"
#include "IO_utils.h"

//------------------------------------------------------------------------------
// Initialize a GPIO pin
//------------------------------------------------------------------------------
int32_t __IO_gpio_init(IO_io *io, uint8_t pin, uint16_t flags, uint8_t dir)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_gpio_init, IO_gpio_init);

//------------------------------------------------------------------------------
// Get state of the pin
//------------------------------------------------------------------------------
int32_t __IO_gpio_get_state(IO_io *io)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_gpio_get_state, IO_gpio_get_state);

//------------------------------------------------------------------------------
// Set state of the pin
//------------------------------------------------------------------------------
void __IO_gpio_set_state(IO_io *io, uint8_t state)
{
}

WEAK_ALIAS(__IO_gpio_set_state, IO_gpio_set_state);
