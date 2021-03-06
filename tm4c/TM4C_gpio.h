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
//! Initialize GPIO port
//------------------------------------------------------------------------------
void TM4C_gpio_port_init(uint8_t port);

//------------------------------------------------------------------------------
//! Initialize GPIO pin
//------------------------------------------------------------------------------
void TM4C_gpio_pin_init(uint8_t port, uint8_t pin, uint8_t afsel,
  uint8_t amsel, uint8_t out);

//------------------------------------------------------------------------------
//! Unlock GPIO pin
//------------------------------------------------------------------------------
void TM4C_gpio_pin_unlock(uint8_t port, uint8_t pin);

//------------------------------------------------------------------------------
// Enable/disable GPIO events
//------------------------------------------------------------------------------
int32_t TM4C_gpio_event_enable(IO_io *io, uint16_t events);
int32_t TM4C_gpio_event_disable(IO_io *io, uint16_t events);
