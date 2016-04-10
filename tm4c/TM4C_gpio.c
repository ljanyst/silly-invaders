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

#include "TM4C.h"
#include "TM4C_gpio.h"

//------------------------------------------------------------------------------
// Initialize GPIO port
//------------------------------------------------------------------------------
void TM4C_gpio_port_init(uint8_t port)
{
  // enable the AHB and GPIO clock
  GPIOHBCTL_REG |= (1 << port);
  RCGCGPIO_REG  |= (1 << port);
}

//------------------------------------------------------------------------------
// Initialize GPIO pin
//------------------------------------------------------------------------------
void TM4C_gpio_pin_init(uint8_t port, uint8_t pin, uint8_t afsel, uint8_t amsel,
  uint8_t out)
{
  uint16_t port_offset = port * GPIO_PORT_OFFSET;

  // enable alternative function of the pin
  if(afsel) {
    GPIO_REG(port_offset, GPIO_AFSEL) |= (1 << pin);

    GPIO_REG(port_offset, GPIO_PCTL) &= ~(0x0f << (pin*4));
    GPIO_REG(port_offset, GPIO_PCTL) |=  ((afsel & 0x0f) << (pin*4));
  }
  else
    GPIO_REG(port_offset, GPIO_AFSEL) &= ~(1 << pin);

  // enable the analog function of the pin
  if(amsel)
    GPIO_REG(port_offset, GPIO_AMSEL) &= ~(1 << pin);
  else
    GPIO_REG(port_offset, GPIO_AMSEL) &= ~(1 << pin);

  // set pin direction
  if(out)
    GPIO_REG(port_offset, GPIO_DIR) |= (1 << pin);
  else
    GPIO_REG(port_offset, GPIO_DIR) &= ~(1 << pin);

  // enable the pin
  GPIO_REG(port_offset, GPIO_DEN) |= (1 << pin);
}

//------------------------------------------------------------------------------
// Unlock GPIO pin
//------------------------------------------------------------------------------
void TM4C_gpio_pin_unlock(uint8_t port, uint8_t pin)
{
  uint16_t port_offset = port * GPIO_PORT_OFFSET;
  GPIO_REG(port_offset, GPIO_LOCK)  = 0x4c4f434b;
  GPIO_REG(port_offset, GPIO_CR)   |= (1 << pin);
}
