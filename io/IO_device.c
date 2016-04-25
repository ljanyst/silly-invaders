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

#include "IO_device.h"
#include "IO_error.h"
#include "IO_utils.h"

//------------------------------------------------------------------------------
// Dummy UART initializer
//------------------------------------------------------------------------------
int32_t __IO_uart_init(IO_io *io, uint8_t module, uint16_t flags, uint32_t baud)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_uart_init, IO_uart_init);

//------------------------------------------------------------------------------
// Initialize a timer
//------------------------------------------------------------------------------
int32_t __IO_timer_init(IO_io *io, uint8_t module)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_timer_init, IO_timer_init);

//------------------------------------------------------------------------------
// Dummy SSI initializer
//------------------------------------------------------------------------------
int32_t __IO_ssi_init(IO_io *io, uint8_t module, uint16_t flags,
  IO_ssi_attrs *attrs)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_ssi_init, IO_ssi_init);

//------------------------------------------------------------------------------
// Initialize an LED
//------------------------------------------------------------------------------
int32_t __IO_led_init(IO_io *io, uint8_t module)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_led_init, IO_led_init);

//------------------------------------------------------------------------------
// Initialize a GPIO pin
//------------------------------------------------------------------------------
int32_t __IO_gpio_init(IO_io *io, uint8_t pin, uint16_t flags, uint8_t dir)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_gpio_init, IO_gpio_init);

//------------------------------------------------------------------------------
// Initialize a button
//------------------------------------------------------------------------------
int32_t __IO_button_init(IO_io *io, uint8_t module, uint16_t flags)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_button_init, IO_button_init);

//------------------------------------------------------------------------------
// Initialize an ADC
//------------------------------------------------------------------------------
int32_t __IO_adc_init(IO_io *io, uint8_t module, uint16_t flags)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_adc_init, IO_adc_init);
